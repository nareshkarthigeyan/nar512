def nPi(n):
    piDigits = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316271201909145648566923460348610454326648213393607260249141273"
    return format(int(piDigits[n % len(piDigits): (n % len(piDigits)) + 4]) % 65536, '016b')

def left_rotate(x, n, w=32):
    n = n % w
    return ((x << n) | (x >> (w - n))) & ((1 << w) - 1)  # Mask to w bits

def right_rotate(x, n, w=32):
    n = n % w
    return ((x >> n) | (x << (w - n))) & ((1 << w) - 1)  # Mask to w

def sigma(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2) 
    a = num ^ piVal
    b = right_rotate(num, 7, 32) ^ left_rotate(num, 3, 16)
    c = (num + piVal) % 4294967295
    result = a ^ b ^ c 
    return format(result % 512, '032b')

def bigM(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2)

    a = right_rotate(num, 23, 32)
    b = left_rotate(num, 11, 32)
    c = (num * piVal) % 512
    res = a ^ b ^ c
    return format(res % 512, '032b')

def padding(input):
    length = len(input)
    res = ''.join(format(ord(i), '08b') for i in input)
    res += "1"
    while len(res) % 512 != 504:
        res += "0"
    res += format(length, '08b')
    return res

def compartmentalize(input):
    res = []
    for i in range(0, len(input), 32):
        res.append(input[i:i+32]);
    return res

def parsing(input):
    res = []
    for i in input:
        res.append(i)

    for i in range(len(input), 64):
        val = int(sigma(input[i % len(input)]), 2) 
        val ^= int(bigM(input[(i + 8) % len(input)]), 2)
        val += left_rotate(val + i * 17, i % 32)
        val += int(res[i - 16], 2)
        val ^= right_rotate(int(res[i - 2], 2) * 19, i % 32)

        res.append(format(val, '032b'))
    return res

def compression(input):
    a = 0x6a09e667
    b = 0xbb67ae85
    c = 0x3c6ef372
    d = 0xa54ff53a
    e = 0x510e527f
    f = 0x9b05688c
    g = 0x1f83d9ab
    h = 0x5be0cd19

    constant = [a, b, c, d, e, f, g, h]
    res = []
    for i in range(len(input)):
        val = int(input[i], 2) + int(sigma(format(constant[(i + 2) % len(constant)], '032b')), 2) + int(bigM(format(constant[(i + 5) % len(constant)], '032b')), 2) + left_rotate(constant[i % len(constant)] * 9, 11)
        res.append(format(val, '08b'))
    return res

def toHash(input):
    hash = ''
    for i in input:
        num = int(i, 2);
        hash += format(num, 'x')
    return hash

def nar512(input):
    padded = padding(input)
    # print(padded)
    parts = compartmentalize(padded)

    parsed = parsing(parts)
    # print(len(parsed))
    # print(toHash(parsed))
    compressed = compression(parsed)
    print(toHash(compressed))


nar512("abcd")