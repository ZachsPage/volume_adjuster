import string

''' Print and force Flush '''
def printF(*args, **kwargs):
    print(*args, **kwargs, flush=True)

''' Print Flush Without a Newline'''
def printFWoNl(*args, **kwargs):
    printF(*args, **kwargs, end=' ')