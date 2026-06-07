
import math

GRAV_ACCEL = 9.81
AIR_GAS_CONSTANT = 287
T_SEA_LEVEL = 288.16
P_SEA_LEVEL = 101325
DENSITY_SEA_LEVEL = 1.225

ATMOSPHERE_LAYERS = [
    (11000, -6.5e-3),
    (26000, 0.0),
    (48000, 3.0e-3),
    (53000, 0.0),
    (79000, -4.5e-3),
    (91000, 4.0e-3),
    (math.inf, 0.0)
]

def get_standard_atmosphere_conditions(altitude: float) -> tuple[float, float, float]:
    """ 
    Calculates the standard atmospheric temperature, pressure, and density
    at a given altitude. 
    
    :param altitude: Altitude to calculate standard atmospheric conditions at (meters) 
    :type altitude: float 
    :return: A tuple of (temperature, pressure, density) at inputed altitude in standard SI units (kelvin, pascals, kg/m^3) 
    :rtype: tuple[float, float, float]
    """
    t = T_SEA_LEVEL
    p = P_SEA_LEVEL
    rho = DENSITY_SEA_LEVEL

    h0 = 0.0

    for h1, lapse_rate in ATMOSPHERE_LAYERS:

        h = min(altitude, h1)

        if lapse_rate == 0:
            p = isothermal_pressure(h, h0, p, t)
            rho = isothermal_density(h, h0, rho, t)

        else:
            T_new = gradient_temp(h, lapse_rate, t, h0)
            theta = T_new / t

            p = gradient_pressure(theta, lapse_rate, p)
            rho = gradient_density(theta, lapse_rate, rho)

            t = T_new

        if altitude <= h1:
            break

        h0 = h1

    return t, p, rho


def gradient_temp(altitude: float, gradient_slope: float, initial_temperature: float, initital_altitude: float) -> float:
    return initial_temperature + gradient_slope * (altitude - initital_altitude)

def gradient_pressure(theta: float, gradient_slope: float, initial_pressure: float) -> float:
    return initial_pressure * theta**(-1*GRAV_ACCEL/(gradient_slope*AIR_GAS_CONSTANT))

def gradient_density(theta: float, gradient_slope: float, initial_density: float) -> float:
    return initial_density * theta**(-1*(GRAV_ACCEL/(gradient_slope*AIR_GAS_CONSTANT) + 1))

def isothermal_pressure(altitude: float, initial_altitude: float, initial_pressure: float, temperature: float) -> float:
    x = (-1*GRAV_ACCEL/(AIR_GAS_CONSTANT*temperature))*(altitude - initial_altitude)
    return initial_pressure * math.exp(x)

def isothermal_density(altitude: float, initial_altitude: float, initial_density: float, temperature: float) -> float:
    x = (-1*GRAV_ACCEL/(AIR_GAS_CONSTANT*temperature))*(altitude - initial_altitude)
    return initial_density * math.exp(x)



if __name__ == "__main__":
    print(get_standard_atmosphere_conditions(0))
    print(get_standard_atmosphere_conditions(9000))
    print(get_standard_atmosphere_conditions(35000))