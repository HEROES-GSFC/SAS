Here there be code for the HEROES SAS flight computer

Instructions for installing the camera's SDK:

    run whatever.run
    run sudo apt-get install expat
    run mkdir ~/wherever/it/goes
    run cp -rf /opt/pleora/ebus_sdk/share/samples/* ~/wherever/it/goes
    edit line 21 of sample.Makefile:
        PUREGEV_ROOT ?= /opt/pleora/ebus_sdk
    edit line 13something of sample.Makefile:
        $(EXEC): $(OBJS)
            $(LD) $(OBJS) -o $@ $(LDFLAGS)
    edit sample.Makefile to link with g++
    create /etc/ld.so.conf.d/imperx.conf with these lines:
        /opt/pleora/ebus_sdk/lib
        /opt/pleora/ebus_sdk/lib/genicam/bin/Linux32_i86
    run sudo ldconfig
    run sudo chmod +x /opt/pleora/ebus_sdk/bin/set_puregev_env
    on startup run source /opt/pleora/ebus_sdk/bin/set_puregev_env



