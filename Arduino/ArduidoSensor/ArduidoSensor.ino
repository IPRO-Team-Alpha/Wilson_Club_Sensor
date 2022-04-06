#include <IMU.h>

float AAx, AAy, AAz;
float ZAx, ZAy, ZAz;
float Vx, Vy, Vz;
float Gx, Gy, Gz;
float AGx, AGy, AGz;
float ZGx, ZGy, ZGz;
float Rx, Ry, Rz;
const int t = 20;
int i = 0;
const float threth = 0.075;

void setup() {
	Serial.begin(19200);
	
	while(!Serial);
	
	if(!IMU.begin()){
		Serial.println("Failed to initialize IMU");
	}
	
	Serial.print("Accel sample rate: ");
	Serial.println(IMU.accelerationSampleRate());
	Serial.print("Gyro sample rate: ");
	Serial.println(IMU.gyroscopeSampleRate());
	
	IMU.readAcceleration(ZAx, ZAy, ZAz);
	IMU.readGyroscope(ZGx, ZGy, ZGx);
}

void loop(){
	AAx = 0;
	AAy = 0;
	AAz = 0;
	AGx = 0;
	AGy = 0;
	AGz = 0;
	while(i < 20){
		IMU.readAcceleration(Ax, Ay, Az);
		IMU.readGyroscope(Gx, Gy, Gz);
		if (abs(Ax) > threth) AAx += Ax - ZAx;
		if (abs(Ay) > threth) AAy += Ay - ZAy;
		if (abs(Az) > threth) AAz += Az - ZAz;
		if (abs(Gx) > threth) AGx += Gx - ZGx;
		if (abs(Gy) > threth) AGy += Gy - ZGy;
		if (abs(Gz) > threth) AGz += Gz - ZGz;
		i++;
	}
	
	i = 0;
	
	Vx += (AAx/20) * t;
	Vy += (AAy/20) * t;
	Vz += (AAz/20) * t;
	
	Rx += (AGx/20) * t;
	Ry += (AGy/20) * t;
	Rz += (AGz/20) * t;

	Serial.print(Vx);
	Serial.print("   ");
	Serial.print(Vy);
	Serial.print("   ");
	Serial.print(Vz);
}
