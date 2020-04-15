# print(f"当前文件夹下总共的代码行数: {sum(sum(sum(1 for line in open(__import__('os').path.join(a,file),encoding='utf-8')) for file in c if file.endswith('.cpp') or file.endswith('.h')) for a,b,c in __import__('os').walk('.'))-1}")
def is_prime(n):
    if n < 1:
        return False
    for i in range(2, n):
        if n % i == 0:
            return False
    return True

def get_solution(m):
    for p in range(2, m):
        for q in range(2, m):
            if is_prime(p) and is_prime(q) and p + q == m:
                print('{} = {} + {}'.format(m, p, q))
                return
            
for m in range(6, 100 + 1, 2):
    get_solution(m)