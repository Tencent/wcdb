BEGIN { 
    FS = " \\| " 
    printf "Test Name"
}
$1 !~ /\[DB BENCHMARK\]/ { next }

$2 == "Begin" {
    testName = $3
    next
}

$2 == "End" {
    testName = ""
    next
}

{
    item = $2
    value = $3
    
    if (!(item in itemMapping)) {
        itemMapping[item] = itemMapping[0] += 1
        printf ",%s", item
    }

    idxItem = itemMapping[item]
    idx = bench[testName][idxItem][0] += 1
    bench[testName][idxItem][idx] = value
}

END {
    printf "\n"
    for (testName in bench) {
        for (itemId in bench[testName]) {
            delete bench[testName][itemId][0]
            total = 0
            count = 0
            for (idx in bench[testName][itemId]) {
                total += bench[testName][itemId][idx]
                count += 1
            }
            delete bench[testName][itemId]
            bench[testName][itemId] = total / count / 1000000
        }

        printf "%s", testName
        for (i = 1; i <= itemMapping[0]; i++) {
            printf ",%.0f", bench[testName][i]
        }
        printf "\n"
    }
}