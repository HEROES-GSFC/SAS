#include "TelemetryStream.hpp"
#include "ImperxStream.hpp"

int main()
{
    TelemetryStream *tStream;
    tStream = new TelemetryStream;

	ImperxStream *sweetThang;
	sweetThang = new ImperxStream;

    sweetThang->Connect();

    sweetThang->Initialize();

	printf("camera temp is %lld\n", sweetThang->getTemperature());
    sweetThang->Disconnect();

    //tStream->test_checksum();
    tStream->make_test_packet( 4 );
	tStream->set_temperature( sweetThang->getTemperature() );
    tStream->print_packet();
    tStream->init_socket();
    tStream->send();
  
    delete tStream;
    exit(0);
}
