function my_sqrt(x)
    if x < 0
        error("Sqrt of negative is not allowed")
    end
    y = x / 4 + 2 / x
    while (y^2 - x) > 1e-12 + x * 1e-12
        y = (y + x / y) / 2
    end
    return y
end

function trapezoid(f, a, b, n)
    I = f(a) + f(b)
    for x in range(a, b, length=n+1)[2:end-1]
        I += 2 * f(x)
    end
    return I * (b - a) / 2n
end
