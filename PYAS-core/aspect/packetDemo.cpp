#include "ImperxStream.hpp"

int main()
{
    TelemetryStream *tStream;

	ImperxStream *sweetThang;
	sweetThang = new ImperxStream;

    sweetThang->Connect();

    sweetThang->Initialize();

	printf("camera temp is %lld\n", sweetThang->getTemperature());
    sweetThang->Disconnect();

    exit(0);
}
