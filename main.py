def padding(input):
    length = len(input)
    res = ''.join(format(ord(i), '08b') for i in input)
    res += "1"
    while len(res) % 512 != 448:
        res += "0"
    res += format(length, '064b')
    return res

def nar512(input):
    print(padding(input))

nar512("abc")