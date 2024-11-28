import numpy as np
import matplotlib.pyplot as plt


def plot_ReLU_Sigmoid():
    # Define x range
    x = np.linspace(-10, 10, 100)

    # Define ReLU function and its derivative
    relu = np.maximum(0, x)
    relu_derivative = np.where(x > 0, 1, 0)

    # Define Sigmoid function and its derivative
    sigmoid = 1 / (1 + np.exp(-x))
    sigmoid_derivative = sigmoid * (1 - sigmoid)

    # Plot ReLU function
    plt.subplot(2, 2, 1)
    plt.plot(x, relu, color="blue")
    plt.grid()
    plt.title("ReLU Function")
    plt.xlabel("x")
    plt.ylabel("ReLU(x)")

    # Plot derivative of ReLU function
    plt.subplot(2, 2, 2)
    plt.plot(x, relu_derivative, color="green")
    plt.grid()
    plt.title("ReLU Derivative")
    plt.xlabel("x")
    plt.ylabel("ReLU'(x)")

    # Plot Sigmoid function
    plt.subplot(2, 2, 3)
    plt.plot(x, sigmoid, color="purple")
    plt.grid()
    plt.title("Sigmoid Function")
    plt.xlabel("x")
    plt.ylabel("Sigmoid(x)")

    # Plot derivative of Sigmoid function
    plt.subplot(2, 2, 4)
    plt.plot(x, sigmoid_derivative, color="orange")
    plt.grid()
    plt.title("Sigmoid Derivative")
    plt.xlabel("x")
    plt.ylabel("Sigmoid'(x)")

    # Adjust layout
    plt.tight_layout()
    plt.show()


def linear_classifier_example():
    # Re-import necessary libraries due to environment reset
    import matplotlib.pyplot as plt
    import numpy as np

    # Define points
    x1 = (2, 1)
    x2 = (3, 4)

    mp = ((x1[0] + x2[0]) / 2, (x1[1] + x2[1]) / 2)
    a_x1_x2 = 3
    b_x1_x2 = -5

    a_exp = -1 / a_x1_x2
    b_exp = mp[1] - a_exp * mp[0]

    x = np.arange(0, 5, 0.1)

    y_exp = a_exp * x + b_exp

    # Plot the points and lines
    fig, ax = plt.subplots()
    ax.plot(x1[0], x1[1], 'bo',
            label=r'$x_{1}$'+"=(2;1) " + r'$y_{1}$' + "=0")
    ax.plot(x2[0], x2[1], 'ro', label=r'$x_{2}$'"=(3;4) " + r'$y_{2}$' + "=1")

    ax.plot(x, y_exp, color='#808080', linestyle='--',
            label="Expected decision boundary")

    # Set labels, grid, and limits
    ax.set_xlabel(r'$x_{n1}$')
    ax.set_ylabel(r'$x_{n2}$')
    ax.set_title(
        "Cartesian Plane with Points " + r'$x_{1}$' + " and " + r'$x_{2}$' + " \nand the expected decision boundary of preceptron training")
    ax.legend()
    ax.grid(True)
    ax.set_xlim(0, 5)
    ax.set_ylim(0, 5)

    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)

    # Display the plot
    plt.show()


def linear_classifier_initial():
    # Re-import necessary libraries due to environment reset
    import matplotlib.pyplot as plt
    import numpy as np

    # Define points
    x1 = (2, 1)
    x2 = (3, 4)

    mp = ((x1[0] + x2[0]) / 2, (x1[1] + x2[1]) / 2)
    a_x1_x2 = 3
    b_x1_x2 = -5

    a_exp = -1 / a_x1_x2
    b_exp = mp[1] - a_exp * mp[0]

    x = np.arange(0, 5, 0.1)

    y_exp = a_exp * x + b_exp

    a_init = 4
    b_init = -2
    y_init = a_init * x + b_init

    # Plot the points and lines
    fig, ax = plt.subplots()
    ax.plot(x1[0], x1[1], 'bo',
            label=r'$x_{1}$'+"=(2;1) " + r'$y_{1}$' + "=0")
    ax.plot(x2[0], x2[1], 'ro', label=r'$x_{2}$'"=(3;4) " + r'$y_{2}$' + "=1")

    ax.plot(x, y_exp, color='#808080', linestyle='--',
            label="Expected decision boundary")

    ax.plot(x, y_init, 'g', label="Initial decision boundary")

    # Set labels, grid, and limits
    ax.set_xlabel(r'$x_{n1}$')
    ax.set_ylabel(r'$x_{n2}$')
    ax.set_title(
        "Cartesian Plane with Points " + r'$x_{1}$' + " and " + r'$x_{2}$' + " \nand expected/initial decision boundaries of classifier training")
    ax.legend()
    ax.grid(True)
    ax.set_xlim(0, 5)
    ax.set_ylim(0, 5)

    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)

    # Display the plot
    plt.show()


def count_next_theta(repeat, lr, w1, w2, b):

    print("INITIAL: ")
    print("(THETA 1): w2 = " + str(-1*w1/w2) + " xn1 + " + str(-1*b/w2))
    loss = 0.5 * (13*w1*w1 + 28*w1*w2 + 10*w1*b - 6*w1 + 17 *
                  w2*w2 + 10*w2*b - 8*w2 + 2*b*b - 2*b + 1)
    print("(LOSS 1): " + str(loss) + "\n")

    for i in range(1, repeat):
        w1 = w1 - lr * (13*w1 + 14*w2 + 5*b - 3)
        w2 = w2 - lr * (14*w1 + 17*w2 + 5*b - 4)
        b = b - lr * (5*w1 + 5*w2 + 2*b - 1)

        # print("w1n: " + str(w1n) + " w2n: " + str(w2n) + " bn: " + str(bn))
        print("(THETA " + str(i+1) + "): w2n = " +
              str(-1*w1/w2) + " xn1 + " + str(-1*b/w2))
        loss = 0.5 * (13*w1*w1 + 28*w1*w2 + 10*w1*b - 6*w1 + 17 *
                      w2*w2 + 10*w2*b - 8*w2 + 2*b*b - 2*b + 1)
        print("(LOSS " + str(i+1) + "): " + str(loss))


def linear_classifier_final():
    # Re-import necessary libraries due to environment reset
    import matplotlib.pyplot as plt
    import numpy as np

    # Define points
    x1 = (2, 1)
    x2 = (3, 4)

    mp = ((x1[0] + x2[0]) / 2, (x1[1] + x2[1]) / 2)
    a_x1_x2 = 3
    b_x1_x2 = -5

    a_exp = -1 / a_x1_x2
    b_exp = mp[1] - a_exp * mp[0]

    x = np.arange(0, 5, 0.1)

    y_exp = a_exp * x + b_exp

    a_init = 4
    b_init = -2
    y_init = a_init * x + b_init

    # w2n = 5.411214953271027 xn1 + -9.822429906542054
    a_final = 5.41
    b_final = -9.82
    y_final = a_final * x + b_final

    # Plot the points and lines
    fig, ax = plt.subplots()
    ax.plot(x1[0], x1[1], 'bo',
            label=r'$x_{1}$'+"=(2;1) " + r'$y_{1}$' + "=0")
    ax.plot(x2[0], x2[1], 'ro', label=r'$x_{2}$'"=(3;4) " + r'$y_{2}$' + "=1")

    ax.plot(x, y_exp, color='#808080', linestyle='--',
            label="Expected decision boundary")

    ax.plot(x, y_init, 'g', label="Initial decision boundary")

    ax.plot(x, y_final, label="Final decision boundary")

    # Set labels, grid, and limits
    ax.set_xlabel(r'$x_{n1}$')
    ax.set_ylabel(r'$x_{n2}$')
    ax.set_title(
        "Cartesian Plane with Points " + r'$x_{1}$' + " and " + r'$x_{2}$' + " \nand expected/initial decision boundaries of classifier training")
    ax.legend()
    ax.grid(True)
    ax.set_xlim(0, 5)
    ax.set_ylim(0, 5)

    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)

    # Display the plot
    plt.show()


# linear_classifier_final()

count_next_theta(1000, 0.001, 8, -2, -4)
# linear_classifier_example()
# linear_classifier_initial()

# w1 = 1
# w2 = 1
# b = 1

# 0.5 * (13*w1*w1 + 28*w1*w2 + 10*w1*b - 6*w1 + 17 *
#        w2*w2 + 10*w2*b - 8*w2 + 2*b*b - 2*b + 1)


# # b=-0.5    w1=0.1  w2=0.3

w2 = ()
