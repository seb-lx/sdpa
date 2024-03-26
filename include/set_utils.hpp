#pragma once

#include <set>
#include <algorithm>


/*
 * Some common set operations to avoid redundancy.
 */
namespace SetOps {
    /*
     * Joins two sets and returns the result.
     */
    template<typename T>
    [[nodiscard]] std::set<T> join_sets(const std::set<T> &set_1, const std::set<T> &set_2) {
        std::set<T> res;
        std::set_union(set_1.begin(), set_1.end(), set_2.begin(), set_2.end(),
                       std::inserter(res, res.begin())
        );
        return std::move(res); // is this implicit - return value optimization?
    }

    /*
     * Calculates set difference, i.e. set_1 without set_2.
     */
    template<typename T>
    [[nodiscard]] std::set<T> diff_sets(const std::set<T> &set_1, const std::set<T> &set_2) {
        std::set<T> res;
        std::set_difference(set_1.begin(), set_1.end(), set_2.begin(), set_2.end(),
                            std::inserter(res, res.begin()));
        return std::move(res);
    }

    /*
     * Calculates set intersection.
     */
    template<typename T>
    [[nodiscard]] std::set<T> intersect_sets(const std::set<T> &set_1, const std::set<T> &set_2) {
        std::set<T> res;
        std::set_intersection(set_1.begin(), set_1.end(), set_2.begin(), set_2.end(),
                              std::inserter(res, res.begin()));
        return std::move(res);
    }

    /*
     * Checks whether the given value is in the set.
     */
    template<typename T>
    [[nodiscard]] bool is_value_in_set(const T &val, const std::set<T> &set) {
        auto it = set.find(val);
        return (it != set.end()) ? true : false;
    }

    /*
     * Checks whether the given value is not in the set.
     */
    template<typename T>
    [[nodiscard]] bool is_value_not_in_set(const T &val, const std::set<T> &set) {
        auto it = set.find(val);
        return (it == set.end()) ? true : false;
    }

    /*
     * Checks whether sets are equal, i.e. contain the same elements.
     */
    template<typename T>
    [[nodiscard]] bool are_sets_equal(const std::set<T> &set_1, const std::set<T> &set_2) {
        bool set_1_in_set_2 = std::includes(set_1.begin(), set_1.end(), set_2.begin(), set_2.end());
        bool set2_in_set_1 = std::includes(set_2.begin(), set_2.end(), set_1.begin(), set_1.end());
        return set_1_in_set_2 && set2_in_set_1;
    }

    /*
     * Checks whether pointer sets are equal, i.e. contain the same underlying elements.
     */
    template<typename T>
    [[nodiscard]] bool
    are_pointer_sets_equal(const std::set<std::shared_ptr<T>> &set_1, const std::set<std::shared_ptr<T>> &set_2) {
        if (set_1.size() != set_2.size()) return false;

        for (const auto &ptr1: set_1) {
            bool found = false;
            for (const auto &ptr2: set_2) {
                if (*ptr1 == *ptr2) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }

        return true;
    }


    // Explicit instantiations
    template std::set<std::shared_ptr<Block>>
    join_sets(const std::set<std::shared_ptr<Block>> &set_1, const std::set<std::shared_ptr<Block>> &set_2);

    template std::set<std::pair<PP, PP>>
    join_sets(const std::set<std::pair<PP, PP>> &set_1, const std::set<std::pair<PP, PP>> &set_2);

    template std::set<Var> join_sets(const std::set<Var> &set_1, const std::set<Var> &set_2);

    template std::set<PP> join_sets(const std::set<PP> &set_1, const std::set<PP> &set_2);

    template std::set<Var> diff_sets(const std::set<Var> &set_1, const std::set<Var> &set_2);

    template std::set<PP> intersect_sets(const std::set<PP> &set_1, const std::set<PP> &set_2);

    template bool is_value_in_set(const PP &val, const std::set<PP> &set);

    template bool is_value_in_set(const std::pair<PP, PP> &val, const std::set<std::pair<PP, PP>> &set);

    template bool is_value_not_in_set(const PP &val, const std::set<PP> &set);

    template bool are_sets_equal(const std::set<PP> &set_1, const std::set<PP> &set_2);

    template bool are_sets_equal(const std::set<Var> &set_1, const std::set<Var> &set_2);

    template bool are_sets_equal(const std::set<std::pair<PP, PP>> &set_1, const std::set<std::pair<PP, PP>> &set_2);

    template bool are_pointer_sets_equal(const std::set<std::shared_ptr<Block>> &set_1,
                                         const std::set<std::shared_ptr<Block>> &set_2);

}
