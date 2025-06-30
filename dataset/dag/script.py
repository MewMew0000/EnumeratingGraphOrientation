l = [3.90267,
     1.3281
     ]
n = len(l)
for i in range(1, n):
    l[i] += l[i - 1]
print(l[-1]/n)  # Changed from // to / for proper division