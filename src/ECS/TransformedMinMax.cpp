#include "TransformedMinMax.hpp"

std::tuple<glm::dvec3, glm::dvec3>
ExpandArea(glm::dvec3 Min, glm::dvec3 Max, double Amount)
{
	glm::dvec3 Mid = glm::mix(Min, Max, 0.5);
	Min = Mid - ((Mid - Min) * Amount);
	Max = Mid + ((Max - Mid) * Amount);
	return {Min, Max};
}