VERSION = 0.35
RELEASE = 1
DEV = 0
PAK_FILE = edgar.pak
DOCS = doc/*
ICONS = icons/

ifeq ($(OS),Windows_NT)
PROG      = edgar.exe
ED_PROG   = mapeditor.exe
PAK_PROG  = pak.exe
PO_PROG   = po_creator.exe
else
PROG      = edgar
ED_PROG   = mapeditor
PAK_PROG  = pak
PO_PROG   = po_creator
endif

CXX       = gcc

PREFIX = $(DESTDIR)/usr
BIN_DIR = $(PREFIX)/games/
DOC_DIR = $(PREFIX)/share/doc/$(PROG)/
ICON_DIR = $(PREFIX)/share/icons/hicolor/
DESKTOP_DIR = $(PREFIX)/share/applications/
LOCALE_DIR = $(PREFIX)/share/locale/
LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

ifeq ($(OS),Windows_NT)
DATA_DIR =
else
DATA_DIR = $(PREFIX)/share/games/edgar/
endif

ifeq ($(DEV),1)
CFLAGS = -Wall -g -pedantic -Werror -DVERSION=$(VERSION) -DRELEASE=$(RELEASE) -DDEV=$(DEV) -DINSTALL_PATH=\"$(DATA_DIR)\" -DLOCALE_DIR=\"$(LOCALE_DIR)\" -DPAK_FILE=\"$(PAK_FILE)\"
else
CFLAGS = -Wall -O2 -pedantic -DVERSION=$(VERSION) -DRELEASE=$(RELEASE) -DDEV=$(DEV) -DINSTALL_PATH=\"$(DATA_DIR)\" -DLOCALE_DIR=\"$(LOCALE_DIR)\" -DPAK_FILE=\"$(PAK_FILE)\"
endif

ifeq ($(OS),Windows_NT)
LFLAGS = `sdl-config --libs` -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf -lz -llibintl -lm
else
LFLAGS = `sdl-config --libs` -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf -lz
endif

PAK_OBJS   = pak_creator.o
PO_OBJS    = po_creator.o
MAIN_OBJS  = draw.o main.o
EDIT_OBJS  = draw_editor.o main_editor.o cursor.o
CORE_OBJS  = animation.o audio.o collisions.o entity.o font.o game.o graphics.o init.o input.o inventory.o
CORE_OBJS += lift.o map.o player.o resources.o weather.o sprites.o bat.o properties.o custom_actions.o
CORE_OBJS += item.o enemies.o hud.o random.o decoration.o chicken_feed.o chicken.o
CORE_OBJS += key_items.o record.o geometry.o chicken_trap.o target.o spawner.o rock.o pressure_plate.o door.o
CORE_OBJS += weak_wall.o switch.o line_def.o boulder_boss.o trigger.o music.o coal_pile.o objective.o level_exit.o
CORE_OBJS += spider.o rock_pile.o grub.o grub_boss.o save_point.o shrub.o projectile.o load_save.o
CORE_OBJS += compress.o global_trigger.o fireball.o wasp.o small_boulder.o dialog.o script.o villager.o
CORE_OBJS += main_menu.o widget.o borgan.o menu.o options_menu.o npc.o gib.o heart_container.o action_point.o
CORE_OBJS += falling_platform.o spitting_plant.o red_grub.o stalactite.o bomb.o jumping_plant.o explosion.o bomb_pile.o
CORE_OBJS += jumping_slime.o egg.o golem_boss.o baby_slime.o spinner.o snail.o floating_snapper.o snake_boss.o
CORE_OBJS += enemy_generator.o flying_bug.o potions.o pak.o control_menu.o label.o sound_menu.o fly_boss.o ant_lion.o
CORE_OBJS += io_menu.o inventory_menu.o scorpion.o status_panel.o trap_door.o arrow_target.o eye_stalk.o snail_shell.o
CORE_OBJS += flying_maggot.o conveyor_belt.o power_generator.o laser_grid.o ceiling_snapper.o gazer.o
CORE_OBJS += teleporter.o sludge.o grabber.o golem_rock_dropper.o yes_no_menu.o about_menu.o summoner.o energy_drainer.o
CORE_OBJS += symbol_block.o number_block.o force_field.o pump.o gazer_eye_slot.o spring.o invulnerable_flyer.o centurion.o
CORE_OBJS += ice_cube.o vanishing_platform.o armadillo.o bubble_machine.o bubble.o map_trigger.o potion_dispenser.o
CORE_OBJS += ceiling_crawler.o tesla_charger.o tesla_pack.o tortoise.o fire_burner.o high_striker.o containment_unit.o
CORE_OBJS += blob_boss.o containment_unit_controls.o ice_spray.o

ifeq ($(OS),Windows_NT)
CORE_OBJS += strtok_r.o
endif

# top-level rule to create the program.
.PHONY: all
all : $(PROG) makefile.dep $(ED_PROG) $(PAK_PROG) $(LOCALE_MO) $(PO_PROG)

makefile.dep : src/*/*.c src/*.c
	for i in src/*.c src/*/*.c; do gcc -MM "$${i}"; done > $@

# compiling other source files.
%.o:
	$(CXX) $(CFLAGS) -c -s $<
	
%.mo: %.po
	msgfmt -c -o $@ $<

# linking the program.
$(PROG): $(MAIN_OBJS) $(CORE_OBJS)
	$(CXX) $(MAIN_OBJS) $(CORE_OBJS) -o $(PROG) $(LFLAGS)
	
# linking the program.
$(ED_PROG): $(EDIT_OBJS) $(CORE_OBJS)
	$(CXX) $(EDIT_OBJS) $(CORE_OBJS) -o $(ED_PROG) $(LFLAGS)

# linking the program.
$(PAK_PROG): $(PAK_OBJS)
	$(CXX) $(PAK_OBJS) -o $(PAK_PROG) $(LFLAGS)
	
# linking the program.
$(PO_PROG): $(PO_OBJS)
	$(CXX) $(PO_OBJS) -o $(PO_PROG) $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(PROG) $(ED_PROG) $(PAK_PROG) $(PO_PROG) $(PAK_FILE) $(LOCALE_MO) *.o makefile.dep
	
buildpak: $(PAK_PROG)
	./$(PAK_PROG) data gfx music sound font $(PAK_FILE)

# install
install: all

	./$(PAK_PROG) data gfx music sound font $(PAK_FILE)

	mkdir -p $(BIN_DIR)
	mkdir -p $(DATA_DIR)
	mkdir -p $(DOC_DIR)
	mkdir -p $(ICON_DIR)16x16/apps
	mkdir -p $(ICON_DIR)32x32/apps
	mkdir -p $(ICON_DIR)64x64/apps
	mkdir -p $(DESKTOP_DIR)

	install -o root -g games -m 755 $(PROG) $(BIN_DIR)$(PROG)
	install -o root -g games -m 644 $(PAK_FILE) $(DATA_DIR)$(PAK_FILE)
	install -o root -g games -m 644 $(DOCS) $(DOC_DIR)
	cp $(ICONS)$(PROG).png $(ICON_DIR)16x16/apps/
	cp $(ICONS)$(PROG).png $(ICON_DIR)32x32/apps/
	cp $(ICONS)$(PROG).png $(ICON_DIR)64x64/apps/
	cp $(ICONS)$(PROG).desktop $(DESKTOP_DIR)

	@for f in $(LOCALE_MO); do \
		lang=`echo $$f | sed -e 's/^locale\///;s/\.mo$$//'`; \
		mkdir -p $(LOCALE_DIR)$$lang/LC_MESSAGES; \
		echo "cp $$f $(LOCALE_DIR)$$lang/LC_MESSAGES/$(PROG).mo"; \
		cp $$f $(LOCALE_DIR)$$lang/LC_MESSAGES/$(PROG).mo; \
	done

uninstall:
	$(RM) $(BIN_DIR)$(PROG)
	$(RM) $(DATA_DIR)$(PAK_FILE)
	$(RM) $(DOC_DIR)*
	$(RM) $(DOC_DIR)
	$(RM) $(ICON_DIR)$(ICONS)$(PROG).png
	$(RM) $(ICON_DIR)16x16/apps/$(PROG).png
	$(RM) $(ICON_DIR)32x32/apps/$(PROG).png
	$(RM) $(ICON_DIR)64x64/apps/$(PROG).png
	$(RM) $(DESKTOP_DIR)$(PROG).desktop

	@for f in $(LOCALE_MO); do \
		lang=`echo $$f | sed -e 's/^locale\///;s/\.mo$$//'`; \
		echo "$(RM) $(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo"; \
		$(RM) $(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo; \
	done

include makefile.dep
