/***************************************
 *  Computer Algebra System SINGULAR   *
 ***************************************/
/*
 * ABSTRACT: resolutions
 */

#include <kernel/GBEngine/syz.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

#include <vector>
#include <map>

#ifdef __GNUC__
#define likely(X)   (__builtin_expect(X, 1))
#define unlikely(X) (__builtin_expect(X, 0))
#else
#define likely(X)   (X)
#define unlikely(X) (X)
#endif

static void update_variables(std::vector<bool> &variables, const ideal L)
{
    const ring R = currRing;
    const int l = IDELEMS(L)-1;
    int k;
    for (int j = R->N; j > 0; j--) {
        if (variables[j-1]) {
            for (k = l; k >= 0; k--) {
                if (p_GetExp(L->m[k], j, R) > 0) {
                    break;
                }
            }
            if (k < 0) {   // no break
                variables[j-1] = false;
            }
        }
    }
}

static inline bool check_variables(const std::vector<bool> variables,
        const poly m)
{
    const ring R = currRing;
    for (int j = R->N; j > 0; j--) {
        if (variables[j-1] && p_GetExp(m, j, R) > 0) {
            return true;
        }
    }
    return false;
}

typedef struct {
    poly lt;
    unsigned long sev;
    int comp;
} lt_struct;

typedef std::vector<lt_struct> lts_vector;
typedef std::map<long, lts_vector> lts_hash;

static void initialize_lts_hash(lts_hash &C, const ideal L)
{
    const ring R = currRing;
    const int n_elems = IDELEMS(L);
    for (int k = 0; k < n_elems; k++) {
        const poly a = L->m[k];
        const long comp = p_GetComp(a, R);
        C[comp].push_back((lt_struct){a, p_GetShortExpVector(a, R), k});
    }
}

#define delete_lts_hash(C) C->clear()

poly find_reducer(const poly multiplier, const poly t,
        const lts_hash *hash_previous_module)
{
    const ring r = currRing;
    lts_hash::const_iterator itr = hash_previous_module->find(p_GetComp(t, r));
    if (itr == hash_previous_module->end()) {
        return NULL;
    }
    lts_vector::const_iterator itr_curr = (itr->second).begin();
    lts_vector::const_iterator itr_end = (itr->second).end();
    const poly q = p_New(r);
    pNext(q) = NULL;
    p_MemSum_LengthGeneral(q->exp, multiplier->exp, t->exp, r->ExpL_Size);
    const unsigned long q_not_sev = ~p_GetShortExpVector(q, r);
    for( ; itr_curr != itr_end; ++itr_curr) {
        if (itr_curr->sev & q_not_sev
                || unlikely(!(_p_LmDivisibleByNoComp(itr_curr->lt, q, r)))) {
            continue;
        }
        p_MemAdd_NegWeightAdjust(q, r);
        p_ExpVectorDiff(q, q, itr_curr->lt, r);
        p_SetComp(q, (unsigned long)(itr_curr->comp)+1, r);
        p_Setm(q, r);
        number n = n_Mult(p_GetCoeff(multiplier, r), p_GetCoeff(t, r), r);
        p_SetCoeff0(q, n_InpNeg(n, r), r);
        return q;
    }
    p_LmFree(q, r);
    return NULL;
}

static poly traverse_tail(poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lts_hash *hash_previous_module);

static inline poly reduce_term(const poly multiplier, const poly term,
        const ideal previous_module, const std::vector<bool> &variables,
        const lts_hash *hash_previous_module)
{
    poly s = find_reducer(multiplier, term, hash_previous_module);
    if (s == NULL) {
        return NULL;
    }
    const ring r = currRing;
    const int c = p_GetComp(s, r) - 1;
    const poly t = traverse_tail(s, c, previous_module, variables,
            hash_previous_module);
    if (t != NULL) {
        s = p_Add_q(s, t, r);
    }
    return s;
}

static poly compute_image(poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lts_hash *hash_previous_module)
{
    const poly tail = previous_module->m[comp]->next;
    if (tail == NULL || !check_variables(variables, multiplier)) {
        return NULL;
    }
    sBucket_pt sum = sBucketCreate(currRing);
    for (poly p = tail; p != NULL; p = pNext(p)) {
        const poly rt = reduce_term(multiplier, p, previous_module, variables,
                hash_previous_module);
        sBucket_Add_p(sum, rt, pLength(rt));
    }
    poly s;
    int l;
    sBucketClearAdd(sum, &s, &l);
    sBucketDestroy(&sum);
    return s;
}

#define CACHE 1

#if CACHE
struct cache_compare
{
    inline bool operator() (const poly& l, const poly& r) const
    {
        return (p_LmCmp(l, r, currRing) == -1);
        /* For expensive orderings, consider:
         * return (memcmp(l->exp, r->exp,
         *         (currRing->CmpL_Size)*sizeof(unsigned long)) < 0);
         */
    }
};

typedef std::map<poly, poly, cache_compare> cache_term;

static cache_term *Cache;

// note: we don't need to keep the coeffs of those terms which are lifted to 0

static void initialize_cache(const int size)
{
    Cache = new cache_term[size];
}

static void delete_cache(const int size)
{
    const ring r = currRing;
    for (int i = 0; i < size; i++) {
        cache_term *T = &(Cache[i]);
        for (cache_term::iterator itr = T->begin(); itr != T->end(); ++itr) {
            p_Delete(&(itr->second), r);
            p_Delete(const_cast<poly*>(&(itr->first)), r);
        }
        T->clear();
    }
    delete[](Cache);
}

static void insert_into_cache_term(cache_term *T, const poly multiplier,
        const poly p)
{
    cache_term::iterator itr = T->find(multiplier);
    if (itr == T->end()) {
        const ring r = currRing;
        T->insert(cache_term::value_type(p_Head(multiplier, r), p_Copy(p, r)));
    }
}

static poly get_from_cache_term(const cache_term::iterator itr,
        const poly multiplier)
{
    if(itr->second == NULL) {
        return NULL;
    }
    const ring r = currRing;
    poly p = p_Copy(itr->second, r);
    if (!n_Equal(pGetCoeff(multiplier), pGetCoeff(itr->first), r)) {
        number n = n_Div(pGetCoeff(multiplier), pGetCoeff(itr->first), r);
        p = p_Mult_nn(p, n, r);
        n_Delete(&n, r);
    }
    return p;
}

static poly traverse_tail(const poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lts_hash *hash_previous_module)
{
    cache_term *T = &(Cache[comp]);
    cache_term::iterator itr = T->find(multiplier);
    if (itr != T->end()) {
        return get_from_cache_term(itr, multiplier);
    }
    poly p = compute_image(multiplier, comp, previous_module, variables,
            hash_previous_module);
    insert_into_cache_term(T, multiplier, p);
    return p;
}
#else
    #define traverse_tail compute_image
#endif   // CACHE

static poly lift_ext_LT(const poly a, const ideal previous_module,
        const std::vector<bool> &variables,
        const lts_hash *hash_previous_module)
{
    const ring R = currRing;
    poly t1 = compute_image(a, p_GetComp(a, R)-1, previous_module, variables,
            hash_previous_module);
    poly t2 = traverse_tail(a->next, p_GetComp(a->next, R)-1, previous_module,
            variables, hash_previous_module);
    t1 = p_Add_q(t1, t2, R);
    return t1;
}

/*****************************************************************************/

// copied from id_DelDiv(), but without testing and without HAVE_RINGS.
// delete id[j], if LT(j) == coeff*mon*LT(i) and vice versa, i.e.,
// delete id[i], if LT(i) == coeff*mon*LT(j)
static void id_DelDiv_no_test(ideal id)
{
    const ring r = currRing;
    int i, j;
    int k = IDELEMS(id)-1;
    for (i = k; i >= 0; i--) {
        if (id->m[i] != NULL) {
            for (j = k; j > i; j--) {
                if (id->m[j] != NULL) {
                    if (p_DivisibleBy(id->m[i], id->m[j], r)) {
                        p_Delete(&id->m[j], r);
                    }
                    else if (p_DivisibleBy(id->m[j], id->m[i], r)) {
                        p_Delete(&id->m[i], r);
                        break;
                    }
                }
            }
        }
    }
}

typedef poly syzHeadFunction(ideal, int, int);

static poly syzHeadFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    return head;
}

static poly syzHeadExtFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    poly head_ext = p_Init(r);
    pSetCoeff0(head_ext, n_InpNeg(n_Div(pGetCoeff(f_i), pGetCoeff(f_j), r->cf),
                r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
        p_SetExp(head_ext, k, lcm-exp_j, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    p_SetComp(head_ext, j+1, r);
    p_Setm(head_ext, r);
    head->next = head_ext;
    return head;
}

typedef ideal syzM_i_Function(ideal, int, syzHeadFunction);

static ideal syzM_i_unsorted(const ideal G, const int i,
        syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int ncols = 0;
    for (int j = i-1; j >= 0; j--) {
        if (pGetComp(G->m[j]) == comp) ncols++;
    }
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        int k = ncols-1;
        for (int j = i-1; j >= 0; j--) {
            if (pGetComp(G->m[j]) == comp) {
                M_i->m[k] = syzHead(G, i, j);
                k--;
            }
        }
        id_DelDiv_no_test(M_i);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal syzM_i_sorted(const ideal G, const int i,
        syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int index = i-1;
    while (pGetComp(G->m[index]) == comp) index--;
    index++;
    int ncols = i-index;
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        for (int j = ncols-1; j >= 0; j--) {
            M_i->m[j] = syzHead(G, i, j+index);
        }
        id_DelDiv_no_test(M_i);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal idConcat(const ideal *M, const int size, const int rank)
{
    int ncols = 0;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            ncols += M[i]->ncols;
        }
    }
    if (ncols == 0) return idInit(1, rank);
    ideal result = idInit(ncols, rank);
    int k = ncols-1;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            for (int j = M[i]->ncols-1; j >= 0; j--) {
                result->m[k] = M[i]->m[j];
                k--;
            }
        }
    }
    return result;
}

static int compare_comp(poly p_a, poly p_b)
{
    const ring r = currRing;
    int comp_a = p_GetComp(p_a, r);
    int comp_b = p_GetComp(p_b, r);
    return (comp_a > comp_b) - (comp_a < comp_b);
}

static int compare_deg(poly p_a, poly p_b)
{
    const ring r = currRing;
    int deg_a = p_Deg(p_a, r);
    int deg_b = p_Deg(p_b, r);
    return (deg_a > deg_b) - (deg_a < deg_b);
}

static int compare_lex(poly p_a, poly p_b)
{
    int cmp;
    const ring r = currRing;
    int exp_a[r->N+1];
    int exp_b[r->N+1];
    p_GetExpV(p_a, exp_a, r);
    p_GetExpV(p_b, exp_b, r);
    for (int i = r->N; i > 0; i--) {
        cmp = (exp_a[i] > exp_b[i]) - (exp_a[i] < exp_b[i]);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0;
}

static int compare_Mi(const void* a, const void *b)
{
    poly p_a = *((poly *)a);
    poly p_b = *((poly *)b);
    int cmp;
    if ((cmp = compare_comp(p_a, p_b))
            || (cmp = compare_deg(p_a, p_b))
            || (cmp = compare_lex(p_a, p_b))) {
        return cmp;
    }
    return 0;
}

static ideal computeFrame(const ideal G, syzM_i_Function syzM_i,
        syzHeadFunction *syzHead)
{
    ideal *M = (ideal *)omalloc((G->ncols-1)*sizeof(ideal));
    for (int i = G->ncols-2; i >= 0; i--) {
        M[i] = syzM_i(G, i+1, syzHead);
    }
    ideal frame = idConcat(M, G->ncols-1, G->ncols);
    for (int i = G->ncols-2; i >= 0; i--) {
        if (M[i] != NULL) {
            omFreeSize(M[i]->m, IDELEMS(M[i])*sizeof(poly));
            omFreeBin(M[i], sip_sideal_bin);
        }
    }
    omFree(M);
    qsort(frame->m, IDELEMS(frame), sizeof(poly), compare_Mi);
    return frame;
}

static void computeLiftings(const resolvente res, const int index,
        std::vector<bool> &variables)
{
    if (index > 1) {   // we don't know if the input is a reduced SB
        update_variables(variables, res[index-1]);
    }
#if CACHE
    initialize_cache(res[index-1]->ncols);
#endif   // CACHE
    lts_hash *hash_previous_module = new lts_hash();
    initialize_lts_hash(*hash_previous_module, res[index-1]);
    for (int j = res[index]->ncols-1; j >= 0; j--) {
        res[index]->m[j]->next->next = lift_ext_LT(res[index]->m[j],
                res[index-1], variables, hash_previous_module);
    }
    delete_lts_hash(hash_previous_module);
    delete(hash_previous_module);
#if CACHE
    delete_cache(res[index-1]->ncols);
#endif   // CACHE
}

static int computeResolution(resolvente &res, const int max_index,
        syzHeadFunction *syzHead, bool do_lifting)
{
    int index = 0;
    if (!idIs0(res[index]) && index < max_index) {
        index++;
        res[index] = computeFrame(res[index-1], syzM_i_unsorted, syzHead);
        std::vector<bool> variables;
        variables.resize(currRing->N, true);
        while (!idIs0(res[index])) {
            if (do_lifting) {
                computeLiftings(res, index, variables);
            }
            if (index >= max_index) { break; }
            index++;
            res[index] = computeFrame(res[index-1], syzM_i_sorted, syzHead);
        }
        variables.clear();
    }
    return index+1;
}

static void set_options(syzHeadFunction **syzHead_ptr, bool *do_lifting_ptr,
        const char *method)
{
    if (strcmp(method, "complete") == 0) {   // default
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = true;
    }
    else if (strcmp(method, "frame") == 0) {
        *syzHead_ptr = syzHeadFrame;
        *do_lifting_ptr = false;
    }
    else if (strcmp(method, "extended frame") == 0) {
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = false;
    }
    else {   // "linear strand" (not yet implemented)
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = true;
    }
}

#define insert_first_term(r, p, q, R)                             \
do                                                                \
{                                                                 \
    p = r;                                                        \
    q = p->next;                                                  \
    if (q != NULL && p_LmCmp(p, q, R) != 1) {                     \
        while (q->next != NULL && p_LmCmp(p, q->next, R) == -1) { \
            pIter(q);                                             \
        }                                                         \
        r = p->next;                                              \
        p->next = q->next;                                        \
        q->next = p;                                              \
    }                                                             \
}                                                                 \
while (0)

static void insert_ext_induced_LTs(const resolvente res, const int length)
{
    const ring R = currRing;
    poly p, q;
    for (int i = length-2; i > 0; i--) {
        for (int j = res[i]->ncols-1; j >= 0; j--) {
            insert_first_term(res[i]->m[j]->next, p, q, R);
            insert_first_term(res[i]->m[j], p, q, R);
        }
    }
}

static void remove_tails(resolvente res)
{
    const ring r = currRing;
    for (int i = 0; i < res[0]->ncols; i++) {
        if (res[0]->m[i]->next != NULL) {
            p_Delete(&(res[0]->m[i]->next), r);
        }
    }
}

syStrategy syFrank(const ideal arg, const int length, const char *method)
{
    syStrategy result = (syStrategy)omAlloc0(sizeof(ssyStrategy));
    resolvente res = (resolvente)omAlloc0((length+1)*sizeof(ideal));
    res[0] = id_Copy(arg, currRing);
    syzHeadFunction *syzHead;
    bool do_lifting;
    set_options(&syzHead, &do_lifting, method);
    int new_length = computeResolution(res, length-1, syzHead, do_lifting);
    if (new_length < length) {
        res = (resolvente)omReallocSize(res, (length+1)*sizeof(ideal),
                (new_length+1)*sizeof(ideal));
    }
    if (strcmp(method, "frame") != 0) {
        insert_ext_induced_LTs(res, new_length);
    } else {
        remove_tails(res);
    }
    result->fullres = res;
    result->length = new_length;
    result->list_length = new_length;
    return result;
}

