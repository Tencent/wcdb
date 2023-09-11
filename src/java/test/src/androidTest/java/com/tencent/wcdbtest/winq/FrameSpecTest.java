// Created by qiuwenchen on 2023/6/9.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.FrameSpec;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class FrameSpecTest {
    @Test
    public void test() {
        winqEqual(new FrameSpec().range().unboundedPreceding(),
                "RANGE UNBOUNDED PRECEDING");
        winqEqual(new FrameSpec().rows().unboundedPreceding(),
                "ROWS UNBOUNDED PRECEDING");
        winqEqual(new FrameSpec().range().preceding(1),
                "RANGE 1 PRECEDING");
        winqEqual(new FrameSpec().range().currentRow(),
                "RANGE CURRENT ROW");
        winqEqual(new FrameSpec().range().betweenUnboundedPreceding().andPreceding(2),
                "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 PRECEDING");
        winqEqual(new FrameSpec().range().betweenPreceding(1).andPreceding(2),
                "RANGE BETWEEN 1 PRECEDING AND 2 PRECEDING");
        winqEqual(new FrameSpec().range().betweenCurrentRow().andPreceding(2),
                "RANGE BETWEEN CURRENT ROW AND 2 PRECEDING");
        winqEqual(new FrameSpec().range().betweenFollowing(1).andPreceding(2),
                "RANGE BETWEEN 1 FOLLOWING AND 2 PRECEDING");
        winqEqual(new FrameSpec().range().betweenUnboundedPreceding().andCurrentRow(),
                "RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW");
        winqEqual(new FrameSpec().range().betweenUnboundedPreceding().andFollowing(2),
                "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 FOLLOWING");
        winqEqual(new FrameSpec().range().betweenUnboundedPreceding().andUnboundedFollowing(),
                "RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING");
    }
}
