#include <set>
#include <ranges>

namespace set_utils {
    template<typename T>
    std::set<T> intersect(const std::set<T>& set1, const std::set<T>& set2) {
        std::set<T> res;
        std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                              std::inserter(res, res.begin()));

        return res;
    }   

    /*

    template<typename T>
    bool intersect(const std::set<T>& set1, const std::set<T>& set2) {
        auto contains_lambda = [&set2](const T& v) {
            return set2.contains(v);
        };

        return std::ranges::any_of(set1, contains_lambda);
    }*/
}