#ifndef _DOF_H_
#define _DOF_H_
class DOF {
private:
	
public:
	float value;
	float min;
	float max;
	void SetValue(float v);
	float GetValue();
	void SetMinMax(float mi, float ma);
	float GetMax();
	float GetMin();
	DOF() {
		value = 0.0f;
		min = -4.0f;
		max = 4.0f;
	}
};
#endif
