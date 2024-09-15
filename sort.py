
import random

def sort_bubble(arr, key):
    res = arr.copy()
    for x in range(len(res)):
        print(len(res)-x-1, "pairs")
        for i in range(len(res)-x-1):
            print(res[i], res[i+1])
            if key(res[i], res[i+1]):
                t = res[i]
                res[i] = res[i+1]
                res[i+1] = t
    return res

a = [1, 2, 3, 4, 5, 6]
random.shuffle(a)
b = sort_bubble(a, lambda x, y: x > y)
print(a, b)
