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

function faketrapezoid(y, a, b, n)
    f(x) = exp(x)
    I = zero(a)
    for x in range(a, b, length=n)
        α = x == a || x == b ? 1 : 2
        I += α * f(x)
    end
    return I * (b - a) / 2 / (n - 1) + y
end

function trapezoid(f, a, b, n)
    I = zero(a)
    for x in range(a, b, length=n)
        α = x == a || x == b ? 1 : 2
        I += α * f(x)
    end
    return I * (b - a) / 2 / (n - 1)
end
