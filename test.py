def longest_common_substring(s1, s2):
    m, n = len(s1), len(s2)
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    
    longest = 0
    longest_end = 0 

    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i - 1] == s2[j - 1]:
                dp[i][j] = dp[i - 1][j - 1] + 1
                if dp[i][j] > longest:
                    longest = dp[i][j]
                    longest_end = i
            else:
                dp[i][j] = 0

    return s1[longest_end - longest: longest_end]

if __name__ == '__main__':
    string1, string2 =  "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a" ,  "222c1ebd08b8bd0fc753062825620c12768de04718ceb705c17e041f9877150b"
    result = longest_common_substring(string1, string2)
    print(result,",", len(result))
