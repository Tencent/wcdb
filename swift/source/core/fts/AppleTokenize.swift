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

import Foundation

public final class AppleTokenizerInfo: TokenizerInfoBase {
    public required init(withArgc argc: Int32, andArgv argv: UnsafePointer<UnsafePointer<Int8>?>?) {}
}

public final class AppleCursorInfo: CursorInfoBase {
    private let input: String
    private let tokenizer: CFStringTokenizer
    private var offset: Int32 = 0
    private var position: Int32 = 0
    private var buffer: [UInt8] = []

    public required init(withInput pInput: UnsafePointer<Int8>?,
                         count: Int32,
                         tokenizerInfo: TokenizerInfoBase) {
        input = pInput != nil ? (String(bytes: pInput!, count: Int(count), encoding: String.Encoding.utf8) ?? "") : ""
        tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault,
                                            input as CFString,
                                            CFRangeMake(0, input.count),
                                            kCFStringTokenizerUnitWord,
                                            CFLocaleCopyCurrent())
    }

    public func step(pToken: inout UnsafePointer<Int8>?,
                     count: inout Int32,
                     startOffset: inout Int32,
                     endOffset: inout Int32,
                     position: inout Int32) -> Int32 {
        let type = CFStringTokenizerAdvanceToNextToken(tokenizer)
        guard type != [] else {
            return SQLITE_DONE
        }

        let range = CFStringTokenizerGetCurrentTokenRange(tokenizer)
        let maxBufferSize = CFStringGetMaximumSizeForEncoding(range.length, CFStringBuiltInEncodings.UTF8.rawValue)
        if maxBufferSize > buffer.count {
            buffer.expand(toNewSize: maxBufferSize)
        }

        var used: Int = 0
        let remaining = UnsafeMutablePointer<Range<String.Index>>.allocate(capacity: 1)
        defer { remaining.deallocate(capacity: 1) }

        guard input.getBytes(&buffer,
                             maxLength: buffer.count,
                             usedLength: &used,
                             encoding: String.Encoding.utf8,
                             range: input.range(location: range.location, length: range.length),
                             remaining: remaining) else {
                                return SQLITE_ERROR
        }

        pToken = buffer.withUnsafeBytes { (bytes) -> UnsafePointer<Int8> in
            return bytes.baseAddress!.assumingMemoryBound(to: Int8.self)
        }

        startOffset = offset
        count = Int32(used)
        endOffset = offset + Int32(used)
        position = self.position + 1

        self.position += 1
        self.offset += Int32(used)

        return SQLITE_OK
    }
}

public final class AppleModule: Module {
    public typealias TokenizerInfo = AppleTokenizerInfo
    public typealias CursorInfo = AppleCursorInfo
    public static let name = "Apple"

    public private(set) static var module = sqlite3_tokenizer_module(
        iVersion: 0,
        xCreate: { (argc, argv, ppTokenizer) -> Int32 in
            return AppleModule.create(argc: argc, argv: argv, ppTokenizer: ppTokenizer)
    },
        xDestroy: { (pTokenizer) -> Int32 in
            return AppleModule.destroy(pTokenizer: pTokenizer)
    },
        xOpen: { (pTokenizer, pInput, nBytes, ppCursor) -> Int32 in
            return AppleModule.open(pTokenizer: pTokenizer, pInput: pInput, nBytes: nBytes, ppCursor: ppCursor)
    },
        xClose: { (pCursor) -> Int32 in
            return AppleModule.close(pCursor: pCursor)
    },
        xNext: { (pCursor, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition) -> Int32 in
            return AppleModule.next(pCursor: pCursor,
                                    ppToken: ppToken,
                                    pnBytes: pnBytes,
                                    piStartOffset: piStartOffset,
                                    piEndOffset: piEndOffset,
                                    piPosition: piPosition)
    },
        xLanguageid: nil)

    public static let address = { () -> Data in
        var pointer = UnsafeMutableRawPointer(&module)
        return Data(bytes: &pointer, count: MemoryLayout.size(ofValue: pointer))
    }()
}

extension Tokenize {
    public static let Apple = Tokenize(module: AppleModule.self)
}
