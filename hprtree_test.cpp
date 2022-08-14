#include <gtest/gtest.h>
#include "HPRTree.h"
#include <vector>

using namespace J;

TEST(HPRTree, HPRTree_Basic) {
    HPRTree<std::size_t> index(259200);

    float x = -180.0f;
    for (std::size_t i = 0; i < (180 * 2 * 2); i++) {
        float y = -90.0f;
        for (std::size_t j = 0; j < (90 * 2 * 2); j++) {
            index.add(i * 1000 + j, {x, y});
            y += 0.5f;
        }
        x += 0.5f;
    }
    index.build();

    std::vector<std::size_t> queryResult;
    queryResult.reserve(1681);
    index.query({-10.0f, 10.0f, -10.0f, 10.0f}, queryResult);

    EXPECT_EQ(queryResult.size(), 1681);
    for(auto& elem : queryResult) {
        std::size_t j = elem % 1000;
        std::size_t i = (elem - j) / 1000;
        EXPECT_LE(j, 200);
        EXPECT_GE(j, 160);
        EXPECT_LE(i, 380);
        EXPECT_GE(i, 340);
    }
}