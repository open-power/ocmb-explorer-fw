app_fw:
	@echo "Building Application FW..."
	@+make -s -C ../apps/app_fw/build
clean:
	@echo "Cleaning Application FW..."
	@+make -s -C ../apps/app_fw/build clean

