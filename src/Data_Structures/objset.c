#include <Data_Structures/objset.h>

bool i_rf_objset_add(struct objset_h *set, void *value)
{
    if (objset_h_get(set, value)) {
        return true;
    }
    return objset_h_add(set, value);
}


// internal version of @ref rf_objset_foreach
#define i_rf_objset_foreach(set_, it_, value_)            \
    for (value_ = objset_h_first((set_), &(it_)->iter);   \
         value_;                                          \
         value_ = objset_h_first((set_), &(it_)->iter))

static bool rf_objset_subset_do(struct objset_h *set1, struct objset_h *set2)
{
    struct rf_objset_iter it1;
    void *elem1;
    struct rf_objset_iter it2;
    void *elem2;
    bool found;

    // search if set1 is a subset of set2
    i_rf_objset_foreach(set1, &it1, elem1) {
        found = false;
        i_rf_objset_foreach(set2, &it2, elem2) {
            if (elem1 == elem2) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool i_rf_objset_subset(struct objset_h *set1, struct objset_h *set2)
{
    if ((objset_empty_(set1) || objset_empty_(set2))) {
        return objset_empty_(set1) && objset_empty_(set2);
    }
    return rf_objset_subset_do(set1, set2);
}

bool i_rf_objset_equal(struct objset_h *set1, struct objset_h *set2)
{
    if ((objset_empty_(set1) || objset_empty_(set2))) {
        return objset_empty_(set1) && objset_empty_(set2);
    }
    //
    return rf_objset_subset_do(set1, set2) && rf_objset_subset_do(set2, set1);
}
