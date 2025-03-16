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
    string1, string2 =  "74ff483e8fce9f62df3c6c9f717d16503994aac039618dc0d1dbbf8acdb25bbc" ,  "9c62f63535ba7a0eb8a7ef4d8176e252e96bfbac5e92eff20ec6e88d08d52ff5"
    result = longest_common_substring(string1, string2)
    print(result,",", len(result))
