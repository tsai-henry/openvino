// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <gtest/gtest.h>

#include <memory>
#include <openvino/core/model.hpp>
#include <openvino/opsets/opset1.hpp>
#include <openvino/pass/manager.hpp>
#include <queue>
#include <string>
#include <transformations/init_node_info.hpp>
#include <transformations/op_conversions/convert_gather_0d.hpp>
#include <transformations/utils/utils.hpp>

#include "common_test_utils/ngraph_test_utils.hpp"

using namespace testing;
using namespace ov;

TEST_F(TransformationTestsF, ConvertGather0DStatic1) {
    {
        auto input = std::make_shared<opset1::Parameter>(element::f32, Shape{6, 12, 10, 24});
        auto indices = std::make_shared<opset1::Parameter>(element::f32, Shape{15, 4, 20, 28});
        auto axis_const = opset1::Constant::create(element::i64, Shape{}, {1});
        auto gather = std::make_shared<opset1::Gather>(input, indices, axis_const);

        model = std::make_shared<Model>(NodeVector{gather}, ParameterVector{input, indices});

        pass::Manager manager;
        manager.register_pass<ov::pass::ConvertGather0D>();
    }

    {
        auto input = std::make_shared<opset1::Parameter>(element::f32, Shape{6, 12, 10, 24});
        auto indices = std::make_shared<opset1::Parameter>(element::f32, Shape{15, 4, 20, 28});
        auto axis_const = opset1::Constant::create(element::i64, Shape{}, {1});
        auto gather = std::make_shared<opset1::Gather>(input, indices, axis_const);

        model_ref = std::make_shared<Model>(NodeVector{gather}, ParameterVector{input, indices});
    }
}

TEST_F(TransformationTestsF, ConvertGather0DStatic2) {
    {
        auto input = std::make_shared<opset1::Parameter>(element::f32, Shape{6, 12, 10, 24});
        auto indices = std::make_shared<opset1::Parameter>(element::f32, Shape{});
        auto axis_const = opset1::Constant::create(element::i64, Shape{}, {1});
        auto gather = std::make_shared<opset1::Gather>(input, indices, axis_const);

        model = std::make_shared<Model>(NodeVector{gather}, ParameterVector{input, indices});
        manager.register_pass<ov::pass::ConvertGather0D>();
    }

    {
        auto input = std::make_shared<opset1::Parameter>(element::f32, Shape{6, 12, 10, 24});
        auto indices = std::make_shared<opset1::Parameter>(element::f32, Shape{});
        auto axis_const = opset1::Constant::create(element::i64, Shape{}, {1});
        auto unsqueeze =
            std::make_shared<opset1::Unsqueeze>(indices, opset1::Constant::create(element::i64, Shape{1}, {0}));
        auto gather = std::make_shared<opset1::Gather>(input, unsqueeze, axis_const);
        auto squeeze = std::make_shared<opset1::Squeeze>(gather, opset1::Constant::create(element::i64, Shape{1}, {1}));

        model_ref = std::make_shared<Model>(NodeVector{squeeze}, ParameterVector{input, indices});
    }
}
