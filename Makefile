# ==================================================================================================
#  COMPILATION
# ==================================================================================================
.PHONY: all # Requires: cmake 3.1.0 or better
all: _build
	@cd _build ; make --no-print-directory -j8

_build: CMakeLists.txt
	@rm -rf _build
	@mkdir _build
	@cd _build ; cmake ..

.PHONY: rebuild-coverage
rebuild-coverage:
	@rm -rf _build
	@mkdir _build
	@cd _build ; cmake -DCOVERAGE_MODE=ON ..
	@make --no-print-directory test

.PHONY: rebuild-debug
rebuild-debug:
	@rm -rf _build
	@mkdir _build
	@cd _build ; cmake -DDEBUG_MODE=ON ..
	@make --no-print-directory

.PHONY: rebuild-release
rebuild-release:
	@rm -rf _build
	@mkdir _build
	@cd _build ; cmake ..
	@make --no-print-directory

.PHONY: clean
clean:
	@rm -rf _build

# ==================================================================================================
#  SUBMODULES
# ==================================================================================================
.PHONY: modules
modules:
	git submodule update --init --recursive

.PHONY: modules-latest
modules-latest:
	git submodule foreach git pull origin master

# ==================================================================================================
#  CODE QUALITY
# ==================================================================================================
.PHONY: format # Requires: clang-format
format:
	@clang-format -i `find src -name *.*pp` `find examples -name *.*pp`

# ==================================================================================================
#  TESTING
# ==================================================================================================
.PHONY: test
test: all
	@echo && _build/basic_test
	@echo && _build/new_test
	@echo && time _build/poisson_gamma
	@echo && _build/example_1_1
	@echo && _build/coin_toss
	@echo && _build/coin_tosses
	@echo && _build/coin_tosses_beta
	@echo && _build/coin_tosses_beta_suff_stat
