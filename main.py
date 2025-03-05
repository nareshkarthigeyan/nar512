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
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p = [
    0xcbbb9d5d, 0x629a292a, 0x9159015a, 0x152fecd8,
    0x67332667, 0x8eb44a87, 0xdb0c2e0d, 0x47b5481d,
    0xe19b48a8, 0xc5c95a63, 0xa1e38b93, 0x29d070bb,
    0x5c26c926, 0x33a7d98e, 0x4aab5db7, 0x4f7a04dd
]

    for i in range(len(input)):
        t1 = (int(input[i], 2) + int(sigma(format(e, '032b')), 2) + int(bigM(format(g, '032b')), 2) + left_rotate(a, 5, 32)) % 4294967295
        t2 = (int(bigM(format(j, '032b')), 2) + int(sigma(format(l, '032b')), 2) + right_rotate(n, 7, 32)) % 4294967295
        p, o, n, m = o, n, m, (l + t1) % 4294967295
        l, k, j, i = k, j, i, (t1 + t2) % 4294967295
        h, g, f, e = g, f, e, (d + t1) % 4294967295
        d, c, b, a = c, b, a, (t1 + t2) % 4294967295

    return [format(x, '08b') for x in (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)]


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
    return toHash(compressed)


string = nar512("NARESH KARTHIGEYAN")

nonce = 0
while not string.startswith("0"):
    new_string = f"{string}{nonce:06d}" 
    string = nar512(new_string)
    print(string)
    nonce += 1

print(string)
print(nonce)