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

public final class WCDBTokenizerInfo: TokenizerInfoBase {
    public required init(withArgc argc: Int32, andArgv argv: UnsafePointer<UnsafePointer<Int8>?>?) {}
}

public final class WCDBCursorInfo: CursorInfoBase {
    private enum TokenType: UInt {
        case basicMultilingualPlaneLetter = 0x00000001
        case basicMultilingualPlaneDigit = 0x00000002
        case basicMultilingualPlaneSymbol = 0x00000003
        case basicMultilingualPlaneOther = 0x0000FFFF
        case auxiliaryPlaneOther = 0xFFFFFFFF
    }

    private let input: UnsafePointer<Int8>!
    private let inputLength: Int32

    private var position: Int32 = 0
    private var startOffset: Int32 = 0
    private var endOffset: Int32 = 0

    private var cursor: Int32 = 0
    private var cursorTokenType: TokenType?
    private var cursorTokenLength: Int32 = 0
    private lazy var symbolCharacterSet: CFCharacterSet? = {
        var characterSet = CFCharacterSetCreateMutable(kCFAllocatorDefault)
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.control))
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.whitespaceAndNewline))
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.nonBase))
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.punctuation))
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.symbol))
        CFCharacterSetUnion(characterSet, CFCharacterSetGetPredefined(CFCharacterSetPredefinedSet.illegal))
        return characterSet
    }()

    private var lemmaBuffer: [UInt8] = []
    private var lemmaBufferLength: Int32 = 0 //>0 lemma is not empty

    private var subTokensLengthArray: [UInt8] = []
    private var subTokensCursor: Int32 = 0
    private var subTokensDoubleChar: Bool = false

    private var buffer: [UInt8] = []
    private var bufferLength: Int32 = 0

    public required init(withInput pInput: UnsafePointer<Int8>?, count: Int32, tokenizerInfo: TokenizerInfoBase) {
        input = pInput
        inputLength = count
    }

    func cursorStep() -> Int32 {
        guard cursor + cursorTokenLength < inputLength else {
            cursor = inputLength
            cursorTokenType = nil
            cursorTokenLength = 0
            return SQLITE_OK
        }
        cursor += cursorTokenLength
        return cursorSetup()
    }

    func cursorSetup() -> Int32 {
        var rc = SQLITE_OK
        let first = UInt8(bitPattern: input.advanced(by: Int(cursor)).pointee)
        switch first {
        case ..<0xC0:
            cursorTokenLength = 1
            switch first {
            case 0x30...0x39:
                cursorTokenType = .basicMultilingualPlaneDigit
            case 0x41...0x5a, 0x61...0x7a:
                cursorTokenType = .basicMultilingualPlaneLetter
            default:
                var result = false
                rc = isSymbol(unicodeChar: UInt16(first), result: &result)
                guard rc == SQLITE_OK else {
                    return rc
                }
                cursorTokenType = result ? .basicMultilingualPlaneSymbol : .basicMultilingualPlaneOther
            }
        case ..<0xF0:
            var unicode: UInt16 = 0
            switch first {
            case ..<0xE0:
                cursorTokenLength = 2
                unicode = UInt16(first & 0x1F)
            default:
                cursorTokenLength = 3
                unicode = UInt16(first & 0x0F)
            }
            for i in cursor+1..<cursor+cursorTokenLength {
                guard i < inputLength else {
                    cursorTokenType = nil
                    cursorTokenLength = inputLength - i
                    return SQLITE_OK
                }
                unicode = (unicode << 6) | UInt16(UInt8(bitPattern: input.advanced(by: Int(i)).pointee) & 0x3F)
            }
            var result = false
            rc = isSymbol(unicodeChar: unicode, result: &result)
            guard rc == SQLITE_OK else {
                return rc
            }
            cursorTokenType = result ? .basicMultilingualPlaneSymbol : .basicMultilingualPlaneOther
        default:
            cursorTokenType = .auxiliaryPlaneOther
            switch first {
            case ..<0xF8:
                cursorTokenLength = 4
            case ..<0xFC:
                cursorTokenLength = 5
            default:
                cursorTokenLength = 6
            }
        }
        return SQLITE_OK
    }

    func isSymbol(unicodeChar: UInt16, result: inout Bool) -> Int32 {
        guard let symbolCharacterSet = self.symbolCharacterSet else {
            return SQLITE_NOMEM
        }
        result = CFCharacterSetIsCharacterMember(symbolCharacterSet, unicodeChar)
        return SQLITE_OK
    }

    static let orthography = NSOrthography(dominantScript: "Latin", languageMap: [ "Latn": ["en"]])
    func lemmatization(input: UnsafePointer<Int8>, inputLength: Int32) -> Int32 {
        if inputLength > buffer.count {
            buffer.expand(toNewSize: Int(inputLength))
        }
        for i in 0..<Int(inputLength) {
            buffer[i] = UInt8(tolower(Int32(input.advanced(by: i).pointee)))
        }
        let optionalString = buffer.withUnsafeBytes { (bytes) -> String? in
            return String(bytes: bytes.baseAddress!.assumingMemoryBound(to: Int8.self),
                          count: Int(inputLength),
                          encoding: .ascii)
        }
        guard let string = optionalString else {
            return SQLITE_OK
        }
        var optionalLemma: String? = nil
        string.enumerateLinguisticTags(in: string.startIndex..<string.endIndex,
                                       scheme: NSLinguisticTagScheme.lemma.rawValue,
                                       options: NSLinguisticTagger.Options.omitWhitespace,
                                       orthography: WCDBCursorInfo.orthography,
                                       invoking: { (tag, _, _, stop) in
                                        optionalLemma = tag.lowercased()
                                        stop = true
        })
        guard let lemma = optionalLemma,
            lemma.count > 0,
            lemma.caseInsensitiveCompare(string) != ComparisonResult.orderedSame else {
            return SQLITE_OK
        }
        lemmaBufferLength = Int32(lemma.count)
        if lemmaBufferLength > lemmaBuffer.capacity {
            lemmaBuffer.expand(toNewSize: Int(lemmaBufferLength))
        }
        memcpy(&lemmaBuffer, lemma.cString, Int(lemmaBufferLength))
        return SQLITE_OK
    }

    func subTokensStep() {
        self.startOffset = self.subTokensCursor
        self.bufferLength = Int32(self.subTokensLengthArray[0])
        if self.subTokensDoubleChar {
            if self.subTokensLengthArray.count > 1 {
                self.bufferLength += Int32(self.subTokensLengthArray[1])
                self.subTokensDoubleChar = false
            } else {
                self.subTokensLengthArray.removeAll()
            }
        } else {
            self.subTokensCursor += Int32(subTokensLengthArray[0])
            self.subTokensLengthArray.removeFirst()
            self.subTokensDoubleChar = true
        }
        self.endOffset = self.startOffset + self.bufferLength
    }

    public func step(pToken: inout UnsafePointer<Int8>?,
                     count: inout Int32,
                     startOffset: inout Int32,
                     endOffset: inout Int32,
                     position: inout Int32) -> Int32 {
        var rc = SQLITE_OK
        if self.position == 0 {
            rc = cursorSetup()
            guard rc == SQLITE_OK else {
                return rc
            }
        }

        if self.lemmaBufferLength == 0 {
            if self.subTokensLengthArray.isEmpty {
                guard self.cursorTokenType != nil else {
                    return SQLITE_DONE
                }

                //Skip symbol
                while self.cursorTokenType == .basicMultilingualPlaneSymbol {
                    rc = cursorStep()
                    guard rc == SQLITE_OK else {
                        return rc
                    }
                }

                guard let tokenType = self.cursorTokenType else {
                    return SQLITE_DONE
                }

                switch tokenType {
                case .basicMultilingualPlaneLetter:
                    fallthrough
                case .basicMultilingualPlaneDigit:
                    self.startOffset = self.cursor
                    repeat {
                        rc = cursorStep()
                    }while(rc == SQLITE_OK && self.cursorTokenType == tokenType)
                    guard rc == SQLITE_OK else {
                        return rc
                    }
                    self.endOffset = self.cursor
                    self.bufferLength = self.endOffset - self.startOffset
                case .basicMultilingualPlaneOther:
                    fallthrough
                case .auxiliaryPlaneOther:
                    self.subTokensLengthArray.append(UInt8(self.cursorTokenLength))
                    self.subTokensCursor = self.cursor
                    self.subTokensDoubleChar = true
                    rc = cursorStep()
                    while rc == SQLITE_OK && self.cursorTokenType == tokenType {
                        self.subTokensLengthArray.append(UInt8(cursorTokenLength))
                        rc = cursorStep()
                    }
                    guard rc == SQLITE_OK else {
                        return rc
                    }
                    subTokensStep()
                default: break
                }
                if tokenType == .basicMultilingualPlaneLetter {
                    rc = lemmatization(input: self.input.advanced(by: Int(self.startOffset)),
                                       inputLength: self.bufferLength)
                    guard rc == SQLITE_OK else {
                        return rc
                    }
                } else {
                    if self.bufferLength > self.buffer.count {
                        self.buffer.expand(toNewSize: Int(self.bufferLength))
                    }
                    memcpy(&self.buffer, input.advanced(by: Int(self.startOffset)), Int(self.bufferLength))
                }
            } else {
                subTokensStep()
                if self.bufferLength > self.buffer.capacity {
                    self.buffer.expand(toNewSize: Int(self.bufferLength))
                }
                memcpy(&self.buffer, input.advanced(by: Int(self.startOffset)), Int(self.bufferLength))
            }

            pToken = self.buffer.withUnsafeBytes { (bytes) -> UnsafePointer<Int8> in
                return bytes.baseAddress!.assumingMemoryBound(to: Int8.self)
            }
            count = self.bufferLength
        } else {
            pToken = self.lemmaBuffer.withUnsafeBytes { (bytes) -> UnsafePointer<Int8> in
                return bytes.baseAddress!.assumingMemoryBound(to: Int8.self)
            }
            count = self.lemmaBufferLength
            self.lemmaBufferLength = 0
        }

        startOffset = self.startOffset
        endOffset = self.endOffset
        self.position += 1
        position = self.position

        return SQLITE_OK
    }
}

public final class WCDBModule: Module {
    public typealias TokenizerInfo = WCDBTokenizerInfo
    public typealias CursorInfo = WCDBCursorInfo

    public static let name = "WCDB"

    public private(set) static var module = sqlite3_tokenizer_module(
        iVersion: 0,
        xCreate: { (argc, argv, ppTokenizer) -> Int32 in
            return WCDBModule.create(argc: argc, argv: argv, ppTokenizer: ppTokenizer)
    },
        xDestroy: { (pTokenizer) -> Int32 in
            return WCDBModule.destroy(pTokenizer: pTokenizer)
    },
        xOpen: { (pTokenizer, pInput, nBytes, ppCursor) -> Int32 in
            return WCDBModule.open(pTokenizer: pTokenizer, pInput: pInput, nBytes: nBytes, ppCursor: ppCursor)
    },
        xClose: { (pCursor) -> Int32 in
            return WCDBModule.close(pCursor: pCursor)
    },
        xNext: { (pCursor, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition) -> Int32 in
            return WCDBModule.next(pCursor: pCursor,
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
    public static let WCDB = Tokenize(module: WCDBModule.self)
}
