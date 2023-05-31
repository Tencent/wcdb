
/* This is the Porter stemming algorithm, coded up in ANSI C by the
 author. It may be be regarded as canonical, in that it follows the
 algorithm presented in
 
 Porter, 1980, An algorithm for suffix stripping, Program, Vol. 14,
 no. 3, pp 130-137,
 
 only differing from it at the points marked --DEPARTURE-- below.
 
 See also http://www.tartarus.org/~martin/PorterStemmer
 
 The algorithm as described in the paper could be exactly replicated
 by adjusting the points of DEPARTURE, but this is barely necessary,
 because (a) the points of DEPARTURE are definitely improvements, and
 (b) no encoding of the Porter stemmer I have seen is anything like
 as exact as this version, even with the points of DEPARTURE!
 
 You can compile it on Unix with 'gcc -O3 -o stem stem.c' after which
 'stem' takes a list of inputs and sends the stemmed equivalent to
 stdout.
 
 The algorithm as encoded here is particularly fast.
 
 Release 1: was many years ago
 Release 2: 11 Apr 2013
 fixes a bug noted by Matt Patenaude <matt@mattpatenaude.com>,
 
 case 'o': if (ends("\03" "ion") && (b[j] == 's' || b[j] == 't')) break;
 ==>
 case 'o': if (ends("\03" "ion") && j >= k0 && (b[j] == 's' || b[j] == 't')) break;
 
 to avoid accessing b[k0-1] when the word in b is "ion".
 Release 3: 25 Mar 2014
 fixes a similar bug noted by Klemens Baum <klemensbaum@gmail.com>,
 that if step1ab leaves a one letter result (ied -> i, aing -> a etc),
 step2 and step4 access the byte before the first letter. So we skip
 steps after step1ab unless k > k0.
 */

#include <stdint.h>
#include <string.h> /* for memmove */

#define TRUE 1
#define FALSE 0

/* The main part of the stemming algorithm starts here. b is a buffer
 holding a word to be stemmed. The letters are in b[k0], b[k0+1] ...
 ending at b[k]. In fact k0 = 0 in this demo program. k is readjusted
 downwards as the stemming progresses. Zero termination is not in fact
 used in the algorithm.
 
 Note that only lower case sequences are stemmed. Forcing to lower case
 should be done before stem(...) is called.
 */

static char* s_b;          /* buffer for word to be stemmed */
static int s_k, s_k0, s_j; /* j is a general offset into the string */

/* cons(i) is TRUE <=> b[i] is a consonant. */

static int cons(int i)
{
    switch (s_b[i]) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return FALSE;
    case 'y':
        return (i == s_k0) ? TRUE : !cons(i - 1);
    default:
        return TRUE;
    }
}

/* m() measures the number of consonant sequences between k0 and j. if c is
 a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
 presence,
 
 <c><v>       gives 0
 <c>vc<v>     gives 1
 <c>vcvc<v>   gives 2
 <c>vcvcvc<v> gives 3
 ....
 */

static int m()
{
    int n = 0;
    int i = s_k0;
    while (TRUE) {
        if (i > s_j) return n;
        if (!cons(i)) break;
        i++;
    }
    i++;
    while (TRUE) {
        while (TRUE) {
            if (i > s_j) return n;
            if (cons(i)) break;
            i++;
        }
        i++;
        n++;
        while (TRUE) {
            if (i > s_j) return n;
            if (!cons(i)) break;
            i++;
        }
        i++;
    }
}

/* vowelinstem() is TRUE <=> k0,...j contains a vowel */

static int vowelinstem()
{
    int i;
    for (i = s_k0; i <= s_j; i++)
        if (!cons(i)) return TRUE;
    return FALSE;
}

/* doublec(j) is TRUE <=> j,(j-1) contain a double consonant. */

static int doublec(int j)
{
    if (j < s_k0 + 1) return FALSE;
    if (s_b[j] != s_b[j - 1]) return FALSE;
    return cons(j);
}

/* cvc(i) is TRUE <=> i-2,i-1,i has the form consonant - vowel - consonant
 and also if the second c is not w,x or y. this is used when trying to
 restore an e at the end of a short word. e.g.
 
 cav(e), lov(e), hop(e), crim(e), but
 snow, box, tray.
 
 */

static int cvc(int i)
{
    if (i < s_k0 + 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return FALSE;
    {
        int ch = s_b[i];
        if (ch == 'w' || ch == 'x' || ch == 'y') return FALSE;
    }
    return TRUE;
}

/* ends(s) is TRUE <=> k0,...k ends with the string s. */

static int ends(char* s)
{
    int length = s[0];
    if (s[length] != s_b[s_k]) return FALSE; /* tiny speed-up */
    if (length > s_k - s_k0 + 1) return FALSE;
    if (memcmp(s_b + s_k - length + 1, s + 1, (size_t) length) != 0)
        return FALSE;
    s_j = s_k - length;
    return TRUE;
}

/* setto(s) sets (j+1),...k to the characters in the string s, readjusting
 k. */

static void setto(char* s)
{
    int length = s[0];
    memmove(s_b + s_j + 1, s + 1, length);
    s_k = s_j + length;
}

/* r(s) is used further down. */

static void r(char* s)
{
    if (m() > 0) setto(s);
}

/* step1ab() gets rid of plurals and -ed or -ing. e.g.
 
 caresses  ->  caress
 ponies    ->  poni
 ties      ->  ti
 caress    ->  caress
 cats      ->  cat
 
 feed      ->  feed
 agreed    ->  agree
 disabled  ->  disable
 
 matting   ->  mat
 mating    ->  mate
 meeting   ->  meet
 milling   ->  mill
 messing   ->  mess
 
 meetings  ->  meet
 
 */

static void step1ab()
{
    if (s_b[s_k] == 's') {
        if (ends("\04"
                 "sses"))
            s_k -= 2;
        else if (ends("\03"
                      "ies"))
            setto(
            "\01"
            "i");
        else if (s_b[s_k - 1] != 's')
            s_k--;
    }
    if (ends("\03"
             "eed")) {
        if (m() > 0) s_k--;
    } else if ((ends("\02"
                     "ed")
                || ends("\03"
                        "ing"))
               && vowelinstem()) {
        s_k = s_j;
        if (ends("\02"
                 "at"))
            setto(
            "\03"
            "ate");
        else if (ends("\02"
                      "bl"))
            setto(
            "\03"
            "ble");
        else if (ends("\02"
                      "iz"))
            setto(
            "\03"
            "ize");
        else if (doublec(s_k)) {
            s_k--;
            {
                int ch = s_b[s_k];
                if (ch == 'l' || ch == 's' || ch == 'z') s_k++;
            }
        } else if (m() == 1 && cvc(s_k))
            setto(
            "\01"
            "e");
    }
}

/* step1c() turns terminal y to i when there is another vowel in the stem. */

static void step1c()
{
    if (ends("\01"
             "y")
        && vowelinstem())
        s_b[s_k] = 'i';
}

/* step2() maps double suffices to single ones. so -ization ( = -ize plus
 -ation) maps to -ize etc. note that the string before the suffix must give
 m() > 0. */

static void step2()
{
    switch (s_b[s_k - 1]) {
    case 'a':
        if (ends("\07"
                 "ational")) {
            r(
            "\03"
            "ate");
            break;
        }
        if (ends("\06"
                 "tional")) {
            r(
            "\04"
            "tion");
            break;
        }
        break;
    case 'c':
        if (ends("\04"
                 "enci")) {
            r(
            "\04"
            "ence");
            break;
        }
        if (ends("\04"
                 "anci")) {
            r(
            "\04"
            "ance");
            break;
        }
        break;
    case 'e':
        if (ends("\04"
                 "izer")) {
            r(
            "\03"
            "ize");
            break;
        }
        break;
    case 'l':
        if (ends("\03"
                 "bli")) {
            r(
            "\03"
            "ble");
            break;
        } /*-DEPARTURE-*/

        /* To match the published algorithm, replace this line with
             case 'l': if (ends("\04" "abli")) { r("\04" "able"); break; } */

        if (ends("\04"
                 "alli")) {
            r(
            "\02"
            "al");
            break;
        }
        if (ends("\05"
                 "entli")) {
            r(
            "\03"
            "ent");
            break;
        }
        if (ends("\03"
                 "eli")) {
            r(
            "\01"
            "e");
            break;
        }
        if (ends("\05"
                 "ousli")) {
            r(
            "\03"
            "ous");
            break;
        }
        break;
    case 'o':
        if (ends("\07"
                 "ization")) {
            r(
            "\03"
            "ize");
            break;
        }
        if (ends("\05"
                 "ation")) {
            r(
            "\03"
            "ate");
            break;
        }
        if (ends("\04"
                 "ator")) {
            r(
            "\03"
            "ate");
            break;
        }
        break;
    case 's':
        if (ends("\05"
                 "alism")) {
            r(
            "\02"
            "al");
            break;
        }
        if (ends("\07"
                 "iveness")) {
            r(
            "\03"
            "ive");
            break;
        }
        if (ends("\07"
                 "fulness")) {
            r(
            "\03"
            "ful");
            break;
        }
        if (ends("\07"
                 "ousness")) {
            r(
            "\03"
            "ous");
            break;
        }
        break;
    case 't':
        if (ends("\05"
                 "aliti")) {
            r(
            "\02"
            "al");
            break;
        }
        if (ends("\05"
                 "iviti")) {
            r(
            "\03"
            "ive");
            break;
        }
        if (ends("\06"
                 "biliti")) {
            r(
            "\03"
            "ble");
            break;
        }
        break;
    case 'g':
        if (ends("\04"
                 "logi")) {
            r(
            "\03"
            "log");
            break;
        } /*-DEPARTURE-*/

        /* To match the published algorithm, delete this line */
    }
}

/* step3() deals with -ic-, -full, -ness etc. similar strategy to step2. */

static void step3()
{
    switch (s_b[s_k]) {
    case 'e':
        if (ends("\05"
                 "icate")) {
            r(
            "\02"
            "ic");
            break;
        }
        if (ends("\05"
                 "ative")) {
            r(
            "\00"
            "");
            break;
        }
        if (ends("\05"
                 "alize")) {
            r(
            "\02"
            "al");
            break;
        }
        break;
    case 'i':
        if (ends("\05"
                 "iciti")) {
            r(
            "\02"
            "ic");
            break;
        }
        break;
    case 'l':
        if (ends("\04"
                 "ical")) {
            r(
            "\02"
            "ic");
            break;
        }
        if (ends("\03"
                 "ful")) {
            r(
            "\00"
            "");
            break;
        }
        break;
    case 's':
        if (ends("\04"
                 "ness")) {
            r(
            "\00"
            "");
            break;
        }
        break;
    }
}

/* step4() takes off -ant, -ence etc., in context <c>vcvc<v>. */

static void step4()
{
    switch (s_b[s_k - 1]) {
    case 'a':
        if (ends("\02"
                 "al"))
            break;
        return;
    case 'c':
        if (ends("\04"
                 "ance"))
            break;
        if (ends("\04"
                 "ence"))
            break;
        return;
    case 'e':
        if (ends("\02"
                 "er"))
            break;
        return;
    case 'i':
        if (ends("\02"
                 "ic"))
            break;
        return;
    case 'l':
        if (ends("\04"
                 "able"))
            break;
        if (ends("\04"
                 "ible"))
            break;
        return;
    case 'n':
        if (ends("\03"
                 "ant"))
            break;
        if (ends("\05"
                 "ement"))
            break;
        if (ends("\04"
                 "ment"))
            break;
        if (ends("\03"
                 "ent"))
            break;
        return;
    case 'o':
        if (ends("\03"
                 "ion")
            && s_j >= s_k0 && (s_b[s_j] == 's' || s_b[s_j] == 't'))
            break;
        if (ends("\02"
                 "ou"))
            break;
        return;
        /* takes care of -ous */
    case 's':
        if (ends("\03"
                 "ism"))
            break;
        return;
    case 't':
        if (ends("\03"
                 "ate"))
            break;
        if (ends("\03"
                 "iti"))
            break;
        return;
    case 'u':
        if (ends("\03"
                 "ous"))
            break;
        return;
    case 'v':
        if (ends("\03"
                 "ive"))
            break;
        return;
    case 'z':
        if (ends("\03"
                 "ize"))
            break;
        return;
    default:
        return;
    }
    if (m() > 1) s_k = s_j;
}

/* step5() removes a final -e if m() > 1, and changes -ll to -l if
 m() > 1. */

static void step5()
{
    s_j = s_k;
    if (s_b[s_k] == 'e') {
        int a = m();
        if (a > 1 || (a == 1 && !cvc(s_k - 1))) s_k--;
    }
    if (s_b[s_k] == 'l' && doublec(s_k) && m() > 1) s_k--;
}

/* In stem(p,i,j), p is a char pointer, and the string to be stemmed is from
 p[i] to p[j] inclusive. Typically i is zero and j is the offset to the last
 character of a string, (p[j+1] == '\0'). The stemmer adjusts the
 characters p[i] ... p[j] and returns the new end-point of the string, k.
 Stemming never increases word length, so i <= k <= j. To turn the stemmer
 into a module, declare 'stem' as extern, and delete the remainder of this
 file.
 */

int porterStem(char* p, int i, int j)
{
    s_b = p;
    s_k = j;
    s_k0 = i;                        /* copy the parameters into statics */
    if (s_k <= s_k0 + 1) return s_k; /*-DEPARTURE-*/

    /* With this line, strings of length 1 or 2 don't go through the
     stemming process, although no mention is made of this in the
     published algorithm. Remove the line to match the published
     algorithm. */

    step1ab();
    if (s_k > s_k0) {
        step1c();
        step2();
        step3();
        step4();
        step5();
    }
    return s_k;
}

/*--------------------stemmer definition ends here------------------------*/
