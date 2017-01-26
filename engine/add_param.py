import sys

def add_param(param, val):
    with open('params/params0.txt', 'a') as f:
        f.write(param + " " + val + "\n")

    with open('params.h', 'r') as f:
        lines = f.readlines()

        ind = 0
        for i, line in enumerate(lines):
            if line.startswith("extern"):
                ind = i

    print ind

    with open('params.h', 'w') as f:
        for i, line in enumerate(lines):
            f.write(line)
            if ind == i:
                f.write('extern double %s;\n' % (param,))

    with open('main.cpp', 'r') as f:
        lines = f.readlines()

        ind = 0
        ind2 = 0
        in_dict = False
        for i, line in enumerate(lines):
            if line.startswith("double") and line.endswith("= 0;\n"):
                ind = i

            if 'param_to_ref = ' in line:
                in_dict = True

            if in_dict and '};' in line:
                ind2 = i - 1
                in_dict = False

    print ind, ind2

    with open('main.cpp', 'w') as f:
        for i, line in enumerate(lines):
            f.write(line)
            if i == ind:
                f.write('double %s = 0;\n' % (param,))
            elif i == ind2:
                f.write('    {"%s", &%s},\n' % (param, param))


if __name__ == '__main__':
    add_param(sys.argv[1], sys.argv[2])
