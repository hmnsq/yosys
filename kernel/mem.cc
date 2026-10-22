/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ಅನ್ತರ್ಭಾವ "kernel/mem.h"
#ಅನ್ತರ್ಭಾವ "kernel/ff.h"

USING_YOSYS_NAMESPACE

ಶೂನ್ಯ Mem::remove() {
	ಆದರೆ (cell) {
		module->remove(cell);
		cell = ಶೂನ್ಯಸೂಚಕ;
	}
	ಆದರೆ (mem) {
		module->memories.erase(mem->name);
		ಅಳಿಸು mem;
		mem = ಶೂನ್ಯಸೂಚಕ;
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports) {
		ಆದರೆ (ಪೋರ್ಟ್.cell) {
			module->remove(ಪೋರ್ಟ್.cell);
			ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
		}
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : wr_ports) {
		ಆದರೆ (ಪೋರ್ಟ್.cell) {
			module->remove(ಪೋರ್ಟ್.cell);
			ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
		}
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits) {
		ಆದರೆ (init.cell) {
			module->remove(init.cell);
			init.cell = ಶೂನ್ಯಸೂಚಕ;
		}
	}
}

ಶೂನ್ಯ Mem::emit() {
	check();
	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> rd_ಎಡ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[i];
		ಆದರೆ (ಪೋರ್ಟ್.removed) {
			ಆದರೆ (ಪೋರ್ಟ್.cell) {
				module->remove(ಪೋರ್ಟ್.cell);
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			rd_ಎಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(i);
		}
	}
	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> wr_ಎಡ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = wr_ports[i];
		ಆದರೆ (ಪೋರ್ಟ್.removed) {
			ಆದರೆ (ಪೋರ್ಟ್.cell) {
				module->remove(ಪೋರ್ಟ್.cell);
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			wr_ಎಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(i);
		}
	}
	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> init_ಎಡ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(inits); i++) {
		ಸ್ವಯಂಚಾಲಿತ &init = inits[i];
		ಆದರೆ (init.removed) {
			ಆದರೆ (init.cell) {
				module->remove(init.cell);
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			init_ಎಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(i);
		}
	}
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ಎಡ); i++)
		ಆದರೆ (i != rd_ಎಡ[i])
			std::ಅದಲುಬದಲು(rd_ports[i], rd_ports[rd_ಎಡ[i]]);
	rd_ports.resize(Get_ಗಾತ್ರ(rd_ಎಡ));
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ಎಡ); i++)
		ಆದರೆ (i != wr_ಎಡ[i])
			std::ಅದಲುಬದಲು(wr_ports[i], wr_ports[wr_ಎಡ[i]]);
	wr_ports.resize(Get_ಗಾತ್ರ(wr_ಎಡ));
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(init_ಎಡ); i++)
		ಆದರೆ (i != init_ಎಡ[i])
			std::ಅದಲುಬದಲು(inits[i], inits[init_ಎಡ[i]]);
	inits.resize(Get_ಗಾತ್ರ(init_ಎಡ));

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ಎಡ); i++) {
			ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[i] = ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[wr_ಎಡ[i]];
			ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[i] = ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[wr_ಎಡ[i]];
		}
		ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(wr_ಎಡ));
		ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(wr_ಎಡ));
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : wr_ports) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ಎಡ); i++)
			ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[i] = ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[wr_ಎಡ[i]];
		ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(wr_ಎಡ));
	}

	ಆದರೆ (packed) {
		ಆದರೆ (mem) {
			module->memories.erase(mem->name);
			ಅಳಿಸು mem;
			mem = ಶೂನ್ಯಸೂಚಕ;
		}
		ಆದರೆ (!cell) {
			ಆದರೆ (memid.ಖಾಲಿಗಳು())
				memid = PooledName(module->design, module->twines().ಕೂಡಿಸು(NEW_ID));
			cell = module->ಕೂಡಿಸು_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::ಅಗಲ_2] = Const(ಅಗಲ);
		cell->parameters[ID::ಸ್ಥಾನಾಂತರ_2] = Const(ಆರಂಭ_ಸ್ಥಾನಾಂತರ);
		cell->parameters[ID::ಗಾತ್ರ_3] = Const(ಗಾತ್ರ_2);
		SigSpec rd_clk, rd_en, rd_addr, rd_ದತ್ತಾಂಶ;
		SigSpec wr_clk, wr_en, wr_addr, wr_ದತ್ತಾಂಶ;
		SigSpec rd_arst, rd_srst;
		ಪೂರ್ಣಾಂಕ abits = 0;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports)
			abits = std::max(abits, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr));
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : wr_ports)
			abits = std::max(abits, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> wr_ಪೋರ್ಟ್_xlat;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++)
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_ಪೋರ್ಟ್_xlat.ತಳ್ಳು_ಹಿಂಭಾಗ(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_ಮುಖವಾಡ_builder;
		Const::Builder rd_collision_x_ಮುಖವಾಡ_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_ಮೌಲ್ಯ_builder;
		Const::Builder rd_srst_ಮೌಲ್ಯ_builder;
		Const::Builder rd_init_ಮೌಲ್ಯ_builder;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ attr: ಪೋರ್ಟ್.ಗುಣಲಕ್ಷಣಗಳು)
				ಆದರೆ (!cell->has_attribute(attr.ಮೊದಲ))
					cell->ಗುಣಲಕ್ಷಣಗಳು.ಸೇರಿಸು(attr);
			ಆದರೆ (ಪೋರ್ಟ್.cell) {
				module->remove(ಪೋರ್ಟ್.cell);
				ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
			}
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << ಪೋರ್ಟ್.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(sub != 0));
				rd_clk_enable_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ಪೋರ್ಟ್.clk_enable));
				rd_clk_polarity_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ಪೋರ್ಟ್.clk_polarity));
				rd_ce_over_srst_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ಪೋರ್ಟ್.ce_over_srst));
				rd_clk.append(ಪೋರ್ಟ್.clk);
				rd_arst.append(ಪೋರ್ಟ್.arst);
				rd_srst.append(ಪೋರ್ಟ್.srst);
				rd_en.append(ಪೋರ್ಟ್.en);
				SigSpec addr = ಪೋರ್ಟ್.sub_addr(sub);
				addr.extend_u0(abits, ಸುಳ್ಳು);
				rd_addr.append(addr);
				log_assert(Get_ಗಾತ್ರ(addr) == abits);
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ idx : wr_ಪೋರ್ಟ್_xlat) {
					rd_transparency_ಮುಖವಾಡ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ತಾರ್ಕಿಕ(ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[idx])));
					rd_collision_x_ಮುಖವಾಡ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ತಾರ್ಕಿಕ(ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[idx])));
				}
			}
			rd_ದತ್ತಾಂಶ.append(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ)
				rd_arst_ಮೌಲ್ಯ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ)
				rd_srst_ಮೌಲ್ಯ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಪೋರ್ಟ್.init_ಮೌಲ್ಯ)
				rd_init_ಮೌಲ್ಯ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_ಮುಖವಾಡ = rd_transparency_ಮುಖವಾಡ_builder.build();
		Const rd_collision_x_ಮುಖವಾಡ = rd_collision_x_ಮುಖವಾಡ_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_ಮೌಲ್ಯ = rd_arst_ಮೌಲ್ಯ_builder.build();
		Const rd_srst_ಮೌಲ್ಯ = rd_srst_ಮೌಲ್ಯ_builder.build();
		Const rd_init_ಮೌಲ್ಯ = rd_init_ಮೌಲ್ಯ_builder.build();
		ಆದರೆ (rd_ports.ಖಾಲಿಗಳು()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_ಮೌಲ್ಯ = State::S0;
			rd_srst_ಮೌಲ್ಯ = State::S0;
			rd_init_ಮೌಲ್ಯ = State::S0;
		}
		ಆದರೆ (rd_ports.ಖಾಲಿಗಳು() || wr_ports.ಖಾಲಿಗಳು()) {
			rd_transparency_ಮುಖವಾಡ = State::S0;
			rd_collision_x_ಮುಖವಾಡ = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_ಗಾತ್ರ(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_ಮುಖವಾಡ] = rd_transparency_ಮುಖವಾಡ;
		cell->parameters[ID::RD_COLLISION_X_ಮುಖವಾಡ] = rd_collision_x_ಮುಖವಾಡ;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_ಮೌಲ್ಯ] = rd_arst_ಮೌಲ್ಯ;
		cell->parameters[ID::RD_SRST_ಮೌಲ್ಯ] = rd_srst_ಮೌಲ್ಯ;
		cell->parameters[ID::RD_INIT_ಮೌಲ್ಯ] = rd_init_ಮೌಲ್ಯ;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_CLK, rd_clk);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_EN, rd_en);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_ARST, rd_arst);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_SRST, rd_srst);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_ADDR, rd_addr);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::RD_ದತ್ತಾಂಶ, rd_ದತ್ತಾಂಶ);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ಆದ್ಯತೆ_ಮುಖವಾಡ_builder;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : wr_ports) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ attr: ಪೋರ್ಟ್.ಗುಣಲಕ್ಷಣಗಳು)
				ಆದರೆ (!cell->has_attribute(attr.ಮೊದಲ))
					cell->ಗುಣಲಕ್ಷಣಗಳು.ಸೇರಿಸು(attr);
			ಆದರೆ (ಪೋರ್ಟ್.cell) {
				module->remove(ಪೋರ್ಟ್.cell);
				ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
			}
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << ಪೋರ್ಟ್.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(sub != 0));
				wr_clk_enable_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ಪೋರ್ಟ್.clk_enable));
				wr_clk_polarity_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ಪೋರ್ಟ್.clk_polarity));
				wr_clk.append(ಪೋರ್ಟ್.clk);
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ idx : wr_ಪೋರ್ಟ್_xlat)
					wr_ಆದ್ಯತೆ_ಮುಖವಾಡ_builder.ತಳ್ಳು_ಹಿಂಭಾಗ(State(ತಾರ್ಕಿಕ(ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[idx])));
				SigSpec addr = ಪೋರ್ಟ್.sub_addr(sub);
				addr.extend_u0(abits, ಸುಳ್ಳು);
				wr_addr.append(addr);
				log_assert(Get_ಗಾತ್ರ(addr) == abits);
			}
			wr_en.append(ಪೋರ್ಟ್.en);
			wr_ದತ್ತಾಂಶ.append(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ಆದ್ಯತೆ_ಮುಖವಾಡ = wr_ಆದ್ಯತೆ_ಮುಖವಾಡ_builder.build();
		ಆದರೆ (wr_ports.ಖಾಲಿಗಳು()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ಆದ್ಯತೆ_ಮುಖವಾಡ = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_ಗಾತ್ರ(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ಆದ್ಯತೆ_ಮುಖವಾಡ] = wr_ಆದ್ಯತೆ_ಮುಖವಾಡ;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->ಗಣ_ಪೋರ್ಟ್(ID::WR_CLK, wr_clk);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::WR_EN, wr_en);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::WR_ADDR, wr_addr);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::WR_ದತ್ತಾಂಶ, wr_ದತ್ತಾಂಶ);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ attr: init.ಗುಣಲಕ್ಷಣಗಳು)
				ಆದರೆ (!cell->has_attribute(attr.ಮೊದಲ))
					cell->ಗುಣಲಕ್ಷಣಗಳು.ಸೇರಿಸು(attr);
			ಆದರೆ (init.cell) {
				module->remove(init.cell);
				init.cell = ಶೂನ್ಯಸೂಚಕ;
			}
		}
		cell->parameters[ID::INIT] = get_init_ದತ್ತಾಂಶ();
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (cell) {
			module->remove(cell);
			cell = ಶೂನ್ಯಸೂಚಕ;
		}
		ಆದರೆ (!mem) {
			ಆದರೆ (memid.ಖಾಲಿಗಳು())
				memid = PooledName(module->design, module->twines().ಕೂಡಿಸು(NEW_ID));
			mem = module->ಕೂಡಿಸು_ಸ್ಮರಣೆ(memid);
		}
		mem->ಅಗಲ = ಅಗಲ;
		mem->ಆರಂಭ_ಸ್ಥಾನಾಂತರ = ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
		mem->ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
		mem->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports) {
			ಆದರೆ (!ಪೋರ್ಟ್.cell)
				ಪೋರ್ಟ್.cell = module->ಕೂಡಿಸು_Cell(NEW_ID, ID($memrd_v2));
			ಪೋರ್ಟ್.cell->type = ID($memrd_v2);
			ಪೋರ್ಟ್.cell->ಗುಣಲಕ್ಷಣಗಳು = ಪೋರ್ಟ್.ಗುಣಲಕ್ಷಣಗಳು;
			ಪೋರ್ಟ್.cell->parameters[ID::MEMID] = memid.str();
			ಪೋರ್ಟ್.cell->parameters[ID::ABITS] = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr);
			ಪೋರ್ಟ್.cell->parameters[ID::ಅಗಲ_2] = ಅಗಲ << ಪೋರ್ಟ್.wide_log2;
			ಪೋರ್ಟ್.cell->parameters[ID::CLK_ENABLE] = ಪೋರ್ಟ್.clk_enable;
			ಪೋರ್ಟ್.cell->parameters[ID::CLK_POLARITY] = ಪೋರ್ಟ್.clk_polarity;
			ಪೋರ್ಟ್.cell->parameters[ID::CE_OVER_SRST] = ಪೋರ್ಟ್.ce_over_srst;
			ಪೋರ್ಟ್.cell->parameters[ID::ARST_ಮೌಲ್ಯ] = ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ;
			ಪೋರ್ಟ್.cell->parameters[ID::SRST_ಮೌಲ್ಯ] = ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ;
			ಪೋರ್ಟ್.cell->parameters[ID::INIT_ಮೌಲ್ಯ] = ಪೋರ್ಟ್.init_ಮೌಲ್ಯ;
			ಪೋರ್ಟ್.cell->parameters[ID::TRANSPARENCY_ಮುಖವಾಡ] = ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ;
			ಪೋರ್ಟ್.cell->parameters[ID::COLLISION_X_ಮುಖವಾಡ] = ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ;
			ಪೋರ್ಟ್.cell->parameters.erase(ID::TRANSPARENT);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, ಪೋರ್ಟ್.clk);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, ಪೋರ್ಟ್.en);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::ARST, ಪೋರ್ಟ್.arst);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::SRST, ಪೋರ್ಟ್.srst);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::ADDR, ಪೋರ್ಟ್.addr);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		}
		ಪೂರ್ಣಾಂಕ idx = 0;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : wr_ports) {
			ಆದರೆ (!ಪೋರ್ಟ್.cell)
				ಪೋರ್ಟ್.cell = module->ಕೂಡಿಸು_Cell(NEW_ID, ID($memwr_v2));
			ಪೋರ್ಟ್.cell->type = ID($memwr_v2);
			ಪೋರ್ಟ್.cell->ಗುಣಲಕ್ಷಣಗಳು = ಪೋರ್ಟ್.ಗುಣಲಕ್ಷಣಗಳು;
			ಆದರೆ (ಪೋರ್ಟ್.cell->parameters.ಎಣಿಕೆ_2(ID::ಆದ್ಯತೆ))
				ಪೋರ್ಟ್.cell->parameters.erase(ID::ಆದ್ಯತೆ);
			ಪೋರ್ಟ್.cell->parameters[ID::MEMID] = memid.str();
			ಪೋರ್ಟ್.cell->parameters[ID::ABITS] = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr);
			ಪೋರ್ಟ್.cell->parameters[ID::ಅಗಲ_2] = ಅಗಲ << ಪೋರ್ಟ್.wide_log2;
			ಪೋರ್ಟ್.cell->parameters[ID::CLK_ENABLE] = ಪೋರ್ಟ್.clk_enable;
			ಪೋರ್ಟ್.cell->parameters[ID::CLK_POLARITY] = ಪೋರ್ಟ್.clk_polarity;
			ಪೋರ್ಟ್.cell->parameters[ID::PORTID] = idx++;
			ಪೋರ್ಟ್.cell->parameters[ID::ಆದ್ಯತೆ_ಮುಖವಾಡ_2] = ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ;
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, ಪೋರ್ಟ್.clk);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, ಪೋರ್ಟ್.en);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::ADDR, ಪೋರ್ಟ್.addr);
			ಪೋರ್ಟ್.cell->ಗಣ_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		}
		idx = 0;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits) {
			ತಾರ್ಕಿಕ v2 = !init.en.is_fully_ones();
			ಆದರೆ (!init.cell)
				init.cell = module->ಕೂಡಿಸು_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (!v2)
					init.cell->unset_ಪೋರ್ಟ್(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->ಗುಣಲಕ್ಷಣಗಳು = init.ಗುಣಲಕ್ಷಣಗಳು;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_ಗಾತ್ರ(init.addr);
			init.cell->parameters[ID::ಅಗಲ_2] = ಅಗಲ;
			init.cell->parameters[ID::ಪದಗಳು] = Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ) / ಅಗಲ;
			init.cell->parameters[ID::ಆದ್ಯತೆ] = idx++;
			init.cell->ಗಣ_ಪೋರ್ಟ್(ID::ADDR, init.addr);
			init.cell->ಗಣ_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, init.ದತ್ತಾಂಶ);
			ಆದರೆ (v2)
				init.cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, init.en);
		}
	}
}

ಶೂನ್ಯ Mem::clear_inits() {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits)
		init.removed = ನಿಜ;
}

ಶೂನ್ಯ Mem::coalesce_inits() {
	// start address -> end address
	std::map<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ> chunks;
	// Figure out chunk boundaries.
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits) {
		ಆದರೆ (init.removed)
			ಮುಂದುವರಿ;
		ತಾರ್ಕಿಕ valid = ಸುಳ್ಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : init.en)
			ಆದರೆ (ಬಿಟ್ == State::S1)
				valid = ನಿಜ;
		ಆದರೆ (!valid) {
			init.removed = ನಿಜ;
			ಮುಂದುವರಿ;
		}
		ಪೂರ್ಣಾಂಕ addr = init.addr.as_int();
		ಪೂರ್ಣಾಂಕ addr_e = addr + Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ) / ಅಗಲ;
		ಸ್ವಯಂಚಾಲಿತ it_e = chunks.ಊರ್ಧ್ವ_bound(addr_e);
		ಸ್ವಯಂಚಾಲಿತ it = it_e;
		ಆಗ (it != chunks.begin()) {
			--it;
			ಆದರೆ (it->ಎರಡನೇ < addr) {
				++it;
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} ಇಲ್ಲದಿದ್ದರೆ {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			ಆದರೆ (it->ಮೊದಲ < addr)
				addr = it->ಮೊದಲ;
			ಸ್ವಯಂಚಾಲಿತ it_ಕೊನೆಯ = it_e;
			it_ಕೊನೆಯ--;
			ಆದರೆ (it_ಕೊನೆಯ->ಎರಡನೇ > addr_e)
				addr_e = it_ಕೊನೆಯ->ಎರಡನೇ;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<ಪೂರ್ಣಾಂಕ, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>> inits_by_chunk;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(inits); i++) {
		ಸ್ವಯಂಚಾಲಿತ &init = inits[i];
		ಆದರೆ (init.removed)
			ಮುಂದುವರಿ;
		ಸ್ವಯಂಚಾಲಿತ it = chunks.ಊರ್ಧ್ವ_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->ಮೊದಲ].ತಳ್ಳು_ಹಿಂಭಾಗ(i);
		ಪೂರ್ಣಾಂಕ addr = init.addr.as_int();
		ಪೂರ್ಣಾಂಕ addr_e = addr + Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ) / ಅಗಲ;
		log_assert(addr >= it->ಮೊದಲ && addr_e <= it->ಎರಡನೇ);
	}
	// Process each chunk.
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : inits_by_chunk) {
		ಪೂರ್ಣಾಂಕ caddr = it.ಮೊದಲ;
		ಪೂರ್ಣಾಂಕ caddr_e = chunks[caddr];
		ಸ್ವಯಂಚಾಲಿತ &chunk_inits = it.ಎರಡನೇ;
		ಆದರೆ (Get_ಗಾತ್ರ(chunk_inits) == 1) {
			ಸ್ವಯಂಚಾಲಿತ &init = inits[chunk_inits[0]];
			ಆದರೆ (!init.en.is_fully_ones()) {
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ); i++)
					ಆದರೆ (init.en[i % ಅಗಲ] != State::S1)
						init.ದತ್ತಾಂಶ.ಗಣ(i, State::Sx);
				init.en = Const(State::S1, ಅಗಲ);
			}
			ಮುಂದುವರಿ;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * ಅಗಲ);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx : chunk_inits) {
			ಸ್ವಯಂಚಾಲಿತ &init = inits[idx];
			ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = (init.addr.as_int() - caddr) * ಅಗಲ;
			log_assert(ಸ್ಥಾನಾಂತರ >= 0);
			log_assert(ಸ್ಥಾನಾಂತರ + Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ) <= Get_ಗಾತ್ರ(cdata));
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ); i++)
				ಆದರೆ (init.en[i % ಅಗಲ] == State::S1)
					cdata.ಗಣ(i+ಸ್ಥಾನಾಂತರ, init.ದತ್ತಾಂಶ[i]);
			init.removed = ನಿಜ;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.ದತ್ತಾಂಶ = cdata;
		new_init.en = Const(State::S1, ಅಗಲ);
		inits.ತಳ್ಳು_ಹಿಂಭಾಗ(new_init);
	}
}

Const Mem::get_init_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ {
	Const init_ದತ್ತಾಂಶ(State::Sx, ಅಗಲ * ಗಾತ್ರ_2);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &init : inits) {
		ಆದರೆ (init.removed)
			ಮುಂದುವರಿ;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = (init.addr.as_int() - ಆರಂಭ_ಸ್ಥಾನಾಂತರ) * ಅಗಲ;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(init.ದತ್ತಾಂಶ); i++)
			ಆದರೆ (0 <= i+ಸ್ಥಾನಾಂತರ && i+ಸ್ಥಾನಾಂತರ < Get_ಗಾತ್ರ(init_ದತ್ತಾಂಶ) && init.en[i % ಅಗಲ] == State::S1)
				init_ದತ್ತಾಂಶ.ಗಣ(i+ಸ್ಥಾನಾಂತರ, init.ದತ್ತಾಂಶ[i]);
	}
	ಹಿಂತಿರುಗಿ init_ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ Mem::check() {
	ಪೂರ್ಣಾಂಕ max_wide_log2 = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : rd_ports) {
		ಆದರೆ (ಪೋರ್ಟ್.removed)
			ಮುಂದುವರಿ;
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.clk) == 1);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.en) == 1);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.arst) == 1);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.srst) == 1);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr) >= ಪೋರ್ಟ್.wide_log2);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.init_ಮೌಲ್ಯ) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		ಆದರೆ (!ಪೋರ್ಟ್.clk_enable) {
			log_assert(ಪೋರ್ಟ್.en == State::S1);
			log_assert(ಪೋರ್ಟ್.arst == State::S0);
			log_assert(ಪೋರ್ಟ್.srst == State::S0);
		}
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಪೋರ್ಟ್.wide_log2; j++) {
			log_assert(ಪೋರ್ಟ್.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, ಪೋರ್ಟ್.wide_log2);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ) == Get_ಗಾತ್ರ(wr_ports));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ) == Get_ಗಾತ್ರ(wr_ports));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(wr_ports); j++) {
			ಸ್ವಯಂಚಾಲಿತ &wport = wr_ports[j];
			ಆದರೆ ((ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[j] || ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[j]) && !wport.removed) {
				log_assert(ಪೋರ್ಟ್.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(ಪೋರ್ಟ್.clk == wport.clk);
				log_assert(ಪೋರ್ಟ್.clk_polarity == wport.clk_polarity);
			}
			log_assert(!ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[j] || !ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[j]);
		}
	}
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = wr_ports[i];
		ಆದರೆ (ಪೋರ್ಟ್.removed)
			ಮುಂದುವರಿ;
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.clk) == 1);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.en) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ) == (ಅಗಲ << ಪೋರ್ಟ್.wide_log2));
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr) >= ಪೋರ್ಟ್.wide_log2);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಪೋರ್ಟ್.wide_log2; j++) {
			log_assert(ಪೋರ್ಟ್.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, ಪೋರ್ಟ್.wide_log2);
		log_assert(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ) == Get_ಗಾತ್ರ(wr_ports));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(wr_ports); j++) {
			ಸ್ವಯಂಚಾಲಿತ &wport = wr_ports[j];
			ಆದರೆ (ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(ಪೋರ್ಟ್.clk_enable == wport.clk_enable);
				ಆದರೆ (ಪೋರ್ಟ್.clk_enable) {
					log_assert(ಪೋರ್ಟ್.clk == wport.clk);
					log_assert(ಪೋರ್ಟ್.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ = (1 << max_wide_log2) - 1;
	log_assert(!(ಆರಂಭ_ಸ್ಥಾನಾಂತರ & ಮುಖವಾಡ));
	log_assert(!(ಗಾತ್ರ_2 & ಮುಖವಾಡ));
	log_assert(ಅಗಲ != 0);
}

ನಾಮಾಕಾಶ {

	ರಚನೆ Mem_ಸೂಚ್ಯಂಕ {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_ಸೂಚ್ಯಂಕ (Module *module) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell: module->cells()) {
				ಆದರೆ (cell->type.ಒಳ(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].ಸೇರಿಸು(cell);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].ಸೇರಿಸು(cell);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].ಸೇರಿಸು(cell);
			}
		}
	};

	Mem mem_from_ಸ್ಮರಣೆ(Module *module, RTLIL::ಸ್ಮರಣೆ *mem, ಸ್ಥಿರಾಂಕ Mem_ಸೂಚ್ಯಂಕ &ಸೂಚ್ಯಂಕ) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->ಅಗಲ, mem->ಆರಂಭ_ಸ್ಥಾನಾಂತರ, mem->ಗಾತ್ರ_2);
		res.packed = ಸುಳ್ಳು;
		res.mem = mem;
		res.ಗುಣಲಕ್ಷಣಗಳು = mem->ಗುಣಲಕ್ಷಣಗಳು;
		std::ಸದಿಶ<ತಾರ್ಕಿಕ> rd_transparent;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> wr_portid;
		ಆದರೆ (ಸೂಚ್ಯಂಕ.rd_ports.ಎಣಿಕೆ_2(memid)) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : ಸೂಚ್ಯಂಕ.rd_ports.at(memid)) {
				MemRd mrd;
				ತಾರ್ಕಿಕ is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.ಗುಣಲಕ್ಷಣಗಳು = cell->ಗುಣಲಕ್ಷಣಗಳು;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_ಪೋರ್ಟ್(ID::CLK);
				mrd.en = cell->get_ಪೋರ್ಟ್(ID::EN);
				mrd.addr = cell->get_ಪೋರ್ಟ್(ID::ADDR);
				mrd.ದತ್ತಾಂಶ = cell->get_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2);
				mrd.wide_log2 = ceil_log2(Get_ಗಾತ್ರ(mrd.ದತ್ತಾಂಶ) / mem->ಅಗಲ);
				ತಾರ್ಕಿಕ transparent = ಸುಳ್ಳು;
				ಆದರೆ (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = ಸುಳ್ಳು;
					mrd.arst_ಮೌಲ್ಯ = Const(State::Sx, mem->ಅಗಲ << mrd.wide_log2);
					mrd.srst_ಮೌಲ್ಯ = Const(State::Sx, mem->ಅಗಲ << mrd.wide_log2);
					mrd.init_ಮೌಲ್ಯ = Const(State::Sx, mem->ಅಗಲ << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					ಆದರೆ (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = ಸುಳ್ಳು;
						ಆದರೆ (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} ಇಲ್ಲದಿದ್ದರೆ {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_ಮೌಲ್ಯ = cell->parameters.at(ID::ARST_ಮೌಲ್ಯ);
					mrd.srst_ಮೌಲ್ಯ = cell->parameters.at(ID::SRST_ಮೌಲ್ಯ);
					mrd.init_ಮೌಲ್ಯ = cell->parameters.at(ID::INIT_ಮೌಲ್ಯ);
					mrd.arst = cell->get_ಪೋರ್ಟ್(ID::ARST);
					mrd.srst = cell->get_ಪೋರ್ಟ್(ID::SRST);
				}
				res.rd_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(mrd);
				rd_transparent.ತಳ್ಳು_ಹಿಂಭಾಗ(transparent);
			}
		}
		ಆದರೆ (ಸೂಚ್ಯಂಕ.wr_ports.ಎಣಿಕೆ_2(memid)) {
			std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, MemWr>> ports;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : ಸೂಚ್ಯಂಕ.wr_ports.at(memid)) {
				MemWr mwr;
				ತಾರ್ಕಿಕ is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.ಗುಣಲಕ್ಷಣಗಳು = cell->ಗುಣಲಕ್ಷಣಗಳು;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_ಪೋರ್ಟ್(ID::CLK);
				mwr.en = cell->get_ಪೋರ್ಟ್(ID::EN);
				mwr.addr = cell->get_ಪೋರ್ಟ್(ID::ADDR);
				mwr.ದತ್ತಾಂಶ = cell->get_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2);
				mwr.wide_log2 = ceil_log2(Get_ಗಾತ್ರ(mwr.ದತ್ತಾಂಶ) / mem->ಅಗಲ);
				ports.ತಳ್ಳು_ಹಿಂಭಾಗ(std::make_pair(cell->parameters.at(is_compat ? ID::ಆದ್ಯತೆ : ID::PORTID).as_int(), mwr));
			}
			std::ವಿಂಗಡಿಸು(ports.begin(), ports.ಕೊನೆ(), [](ಸ್ಥಿರಾಂಕ std::pair<ಪೂರ್ಣಾಂಕ, MemWr> &a, ಸ್ಥಿರಾಂಕ std::pair<ಪೂರ್ಣಾಂಕ, MemWr> &b) { ಹಿಂತಿರುಗಿ a.ಮೊದಲ < b.ಮೊದಲ; });
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : ports) {
				res.wr_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
				wr_portid.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಮೊದಲ);
			}
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(res.wr_ports); i++) {
				ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = res.wr_ports[i];
				ತಾರ್ಕಿಕ is_compat = ಪೋರ್ಟ್.cell->type == ID($memwr);
				ಆದರೆ (is_compat) {
					ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < i; j++) {
						ಸ್ವಯಂಚಾಲಿತ &oport = res.wr_ports[j];
						ಆದರೆ (ಪೋರ್ಟ್.clk_enable != oport.clk_enable)
							ಮುಂದುವರಿ;
						ಆದರೆ (ಪೋರ್ಟ್.clk_enable && ಪೋರ್ಟ್.clk != oport.clk)
							ಮುಂದುವರಿ;
						ಆದರೆ (ಪೋರ್ಟ್.clk_enable && ಪೋರ್ಟ್.clk_polarity != oport.clk_polarity)
							ಮುಂದುವರಿ;
						ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[j] = ನಿಜ;
					}
				} ಇಲ್ಲದಿದ್ದರೆ {
					Const orig_prio_ಮುಖವಾಡ = ಪೋರ್ಟ್.cell->parameters.at(ID::ಆದ್ಯತೆ_ಮುಖವಾಡ_2);
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ orig_portid : wr_portid) {
						ತಾರ್ಕಿಕ has_prio = orig_portid < Get_ಗಾತ್ರ(orig_prio_ಮುಖವಾಡ) && orig_prio_ಮುಖವಾಡ[orig_portid] == State::S1;
						ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(has_prio);
					}
				}
			}
		}
		ಆದರೆ (ಸೂಚ್ಯಂಕ.inits.ಎಣಿಕೆ_2(memid)) {
			std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, MemInit>> inits;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : ಸೂಚ್ಯಂಕ.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.ಗುಣಲಕ್ಷಣಗಳು = cell->ಗುಣಲಕ್ಷಣಗಳು;
				ಸ್ವಯಂಚಾಲಿತ addr = cell->get_ಪೋರ್ಟ್(ID::ADDR);
				ಸ್ವಯಂಚಾಲಿತ ದತ್ತಾಂಶ = cell->get_ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2);
				ಆದರೆ (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				ಆದರೆ (!ದತ್ತಾಂಶ.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(ದತ್ತಾಂಶ), cell);
				init.addr = addr.as_const();
				init.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ.as_const();
				ಆದರೆ (cell->type == ID($meminit_v2)) {
					ಸ್ವಯಂಚಾಲಿತ en = cell->get_ಪೋರ್ಟ್(ID::EN);
					ಆದರೆ (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} ಇಲ್ಲದಿದ್ದರೆ {
					init.en = RTLIL::Const(State::S1, mem->ಅಗಲ);
				}
				inits.ತಳ್ಳು_ಹಿಂಭಾಗ(std::make_pair(cell->parameters.at(ID::ಆದ್ಯತೆ).as_int(), init));
			}
			std::ವಿಂಗಡಿಸು(inits.begin(), inits.ಕೊನೆ(), [](ಸ್ಥಿರಾಂಕ std::pair<ಪೂರ್ಣಾಂಕ, MemInit> &a, ಸ್ಥಿರಾಂಕ std::pair<ಪೂರ್ಣಾಂಕ, MemInit> &b) { ಹಿಂತಿರುಗಿ a.ಮೊದಲ < b.ಮೊದಲ; });
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : inits)
				res.inits.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
		}
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(res.rd_ports); i++) {
			ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = res.rd_ports[i];
			ತಾರ್ಕಿಕ is_compat = ಪೋರ್ಟ್.cell->type == ID($memrd);
			ಆದರೆ (is_compat) {
				ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
				ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
				ಆದರೆ (!rd_transparent[i])
					ಮುಂದುವರಿ;
				ಆದರೆ (!ಪೋರ್ಟ್.clk_enable)
					ಮುಂದುವರಿ;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(res.wr_ports); j++) {
					ಸ್ವಯಂಚಾಲಿತ &wport = res.wr_ports[j];
					ಆದರೆ (!wport.clk_enable)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk != wport.clk)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk_polarity != wport.clk_polarity)
						ಮುಂದುವರಿ;
					ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[j] = ನಿಜ;
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				Const orig_trans_ಮುಖವಾಡ = ಪೋರ್ಟ್.cell->parameters.at(ID::TRANSPARENCY_ಮುಖವಾಡ);
				Const orig_cx_ಮುಖವಾಡ = ಪೋರ್ಟ್.cell->parameters.at(ID::COLLISION_X_ಮುಖವಾಡ);
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ orig_portid : wr_portid) {
					ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(orig_portid < Get_ಗಾತ್ರ(orig_trans_ಮುಖವಾಡ) && orig_trans_ಮುಖವಾಡ[orig_portid] == State::S1);
					ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(orig_portid < Get_ಗಾತ್ರ(orig_cx_ಮುಖವಾಡ) && orig_cx_ಮುಖವಾಡ[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		ಹಿಂತಿರುಗಿ res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().ಕೂಡಿಸು(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::ಅಗಲ_2).as_int(),
			cell->parameters.at(ID::ಸ್ಥಾನಾಂತರ_2).as_int(),
			cell->parameters.at(ID::ಗಾತ್ರ_3).as_int()
		);
		ತಾರ್ಕಿಕ is_compat = cell->type == ID($mem);
		ಪೂರ್ಣಾಂಕ abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = ನಿಜ;
		res.cell = cell;
		res.ಗುಣಲಕ್ಷಣಗಳು = cell->ಗುಣಲಕ್ಷಣಗಳು;
		Const &init = cell->parameters.at(ID::INIT);
		ಆದರೆ (!init.is_fully_undef()) {
			ಪೂರ್ಣಾಂಕ pos = 0;
			ಆಗ (pos < res.ಗಾತ್ರ_2) {
				Const ಶಬ್ದ = init.ಹೊರತೆಗೆ(pos * res.ಅಗಲ, res.ಅಗಲ, State::Sx);
				ಆದರೆ (ಶಬ್ದ.is_fully_undef()) {
					pos++;
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಪೂರ್ಣಾಂಕ epos;
					ಗಾಗಿ (epos = pos; epos < res.ಗಾತ್ರ_2; epos++) {
						Const eword = init.ಹೊರತೆಗೆ(epos * res.ಅಗಲ, res.ಅಗಲ, State::Sx);
						ಆದರೆ (eword.is_fully_undef())
							ನಿಲ್ಲಿಸು;
					}
					MemInit minit;
					minit.addr = res.ಆರಂಭ_ಸ್ಥಾನಾಂತರ + pos;
					minit.ದತ್ತಾಂಶ = init.ಹೊರತೆಗೆ(pos * res.ಅಗಲ, (epos - pos) * res.ಅಗಲ, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.ಅಗಲ);
					res.inits.ತಳ್ಳು_ಹಿಂಭಾಗ(minit);
					pos = epos;
				}
			}
		}
		ಪೂರ್ಣಾಂಕ n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		ಪೂರ್ಣಾಂಕ n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			ಆಗ (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).ಹೊರತೆಗೆ(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).ಹೊರತೆಗೆ(i, 1).as_bool();
			mrd.clk = cell->get_ಪೋರ್ಟ್(ID::RD_CLK).ಹೊರತೆಗೆ(i, 1);
			mrd.en = cell->get_ಪೋರ್ಟ್(ID::RD_EN).ಹೊರತೆಗೆ(i, 1);
			mrd.addr = cell->get_ಪೋರ್ಟ್(ID::RD_ADDR).ಹೊರತೆಗೆ(i * abits, abits);
			mrd.ದತ್ತಾಂಶ = cell->get_ಪೋರ್ಟ್(ID::RD_ದತ್ತಾಂಶ).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
			ಆದರೆ (is_compat) {
				mrd.ce_over_srst = ಸುಳ್ಳು;
				mrd.arst_ಮೌಲ್ಯ = Const(State::Sx, res.ಅಗಲ << mrd.wide_log2);
				mrd.srst_ಮೌಲ್ಯ = Const(State::Sx, res.ಅಗಲ << mrd.wide_log2);
				mrd.init_ಮೌಲ್ಯ = Const(State::Sx, res.ಅಗಲ << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} ಇಲ್ಲದಿದ್ದರೆ {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).ಹೊರತೆಗೆ(i, 1).as_bool();
				mrd.arst_ಮೌಲ್ಯ = cell->parameters.at(ID::RD_ARST_ಮೌಲ್ಯ).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
				mrd.srst_ಮೌಲ್ಯ = cell->parameters.at(ID::RD_SRST_ಮೌಲ್ಯ).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
				mrd.init_ಮೌಲ್ಯ = cell->parameters.at(ID::RD_INIT_ಮೌಲ್ಯ).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
				mrd.arst = cell->get_ಪೋರ್ಟ್(ID::RD_ARST).ಹೊರತೆಗೆ(i, 1);
				mrd.srst = cell->get_ಪೋರ್ಟ್(ID::RD_SRST).ಹೊರತೆಗೆ(i, 1);
			}
			ಆದರೆ (!is_compat) {
				Const transparency_ಮುಖವಾಡ = cell->parameters.at(ID::RD_TRANSPARENCY_ಮುಖವಾಡ).ಹೊರತೆಗೆ(i * n_wr_ports, n_wr_ports);
				Const collision_x_ಮುಖವಾಡ = cell->parameters.at(ID::RD_COLLISION_X_ಮುಖವಾಡ).ಹೊರತೆಗೆ(i * n_wr_ports, n_wr_ports);
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < n_wr_ports; j++)
					ಆದರೆ (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(transparency_ಮುಖವಾಡ[j] == State::S1);
						mrd.collision_x_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(collision_x_ಮುಖವಾಡ[j] == State::S1);
					}
			}
			res.rd_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(mrd);
		}
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			ಆಗ (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).ಹೊರತೆಗೆ(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).ಹೊರತೆಗೆ(i, 1).as_bool();
			mwr.clk = cell->get_ಪೋರ್ಟ್(ID::WR_CLK).ಹೊರತೆಗೆ(i, 1);
			mwr.en = cell->get_ಪೋರ್ಟ್(ID::WR_EN).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
			mwr.addr = cell->get_ಪೋರ್ಟ್(ID::WR_ADDR).ಹೊರತೆಗೆ(i * abits, abits);
			mwr.ದತ್ತಾಂಶ = cell->get_ಪೋರ್ಟ್(ID::WR_ದತ್ತಾಂಶ).ಹೊರತೆಗೆ(i * res.ಅಗಲ, (ni - i) * res.ಅಗಲ);
			ಆದರೆ (!is_compat) {
				Const ಆದ್ಯತೆ_ಮುಖವಾಡ = cell->parameters.at(ID::WR_ಆದ್ಯತೆ_ಮುಖವಾಡ).ಹೊರತೆಗೆ(i * n_wr_ports, n_wr_ports);
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < n_wr_ports; j++)
					ಆದರೆ (wr_wide_continuation[j] != State::S1)
						mwr.ಆದ್ಯತೆ_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಆದ್ಯತೆ_ಮುಖವಾಡ[j] == State::S1);
			}
			res.wr_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(mwr);
		}
		ಆದರೆ (is_compat) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(res.wr_ports); i++) {
				ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = res.wr_ports[i];
				ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < i; j++) {
					ಸ್ವಯಂಚಾಲಿತ &oport = res.wr_ports[j];
					ಆದರೆ (ಪೋರ್ಟ್.clk_enable != oport.clk_enable)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk_enable && ಪೋರ್ಟ್.clk != oport.clk)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk_enable && ಪೋರ್ಟ್.clk_polarity != oport.clk_polarity)
						ಮುಂದುವರಿ;
					ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ[j] = ನಿಜ;
				}
			}
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(res.rd_ports); i++) {
				ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = res.rd_ports[i];
				ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
				ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.resize(Get_ಗಾತ್ರ(res.wr_ports));
				ಆದರೆ (!cell->parameters.at(ID::RD_TRANSPARENT).ಹೊರತೆಗೆ(i, 1).as_bool())
					ಮುಂದುವರಿ;
				ಆದರೆ (!ಪೋರ್ಟ್.clk_enable)
					ಮುಂದುವರಿ;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(res.wr_ports); j++) {
					ಸ್ವಯಂಚಾಲಿತ &wport = res.wr_ports[j];
					ಆದರೆ (!wport.clk_enable)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk != wport.clk)
						ಮುಂದುವರಿ;
					ಆದರೆ (ಪೋರ್ಟ್.clk_polarity != wport.clk_polarity)
						ಮುಂದುವರಿ;
					ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[j] = ನಿಜ;
				}
			}
		}
		res.check();
		ಹಿಂತಿರುಗಿ res;
	}

}

std::ಸದಿಶ<Mem> Mem::get_all_memories(Module *module) {
	std::ಸದಿಶ<Mem> res;
	Mem_ಸೂಚ್ಯಂಕ ಸೂಚ್ಯಂಕ(module);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it: module->memories) {
		res.ತಳ್ಳು_ಹಿಂಭಾಗ(mem_from_ಸ್ಮರಣೆ(module, it.ಎರಡನೇ, ಸೂಚ್ಯಂಕ));
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell: module->cells()) {
		ಆದರೆ (cell->type.ಒಳ(ID($mem), ID($mem_v2)))
			res.ತಳ್ಳು_ಹಿಂಭಾಗ(mem_from_cell(cell));
	}
	ಹಿಂತಿರುಗಿ res;
}

std::ಸದಿಶ<Mem> Mem::get_selected_memories(Module *module) {
	std::ಸದಿಶ<Mem> res;
	Mem_ಸೂಚ್ಯಂಕ ಸೂಚ್ಯಂಕ(module);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it: module->memories) {
		ಆದರೆ (module->design->selected(module, it.ಎರಡನೇ))
			res.ತಳ್ಳು_ಹಿಂಭಾಗ(mem_from_ಸ್ಮರಣೆ(module, it.ಎರಡನೇ, ಸೂಚ್ಯಂಕ));
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell: module->selected_cells()) {
		ಆದರೆ (cell->type.ಒಳ(ID($mem), ID($mem_v2)))
			res.ತಳ್ಳು_ಹಿಂಭಾಗ(mem_from_cell(cell));
	}
	ಹಿಂತಿರುಗಿ res;
}

Cell *Mem::ಹೊರತೆಗೆ_rdff(ಪೂರ್ಣಾಂಕ idx, FfInitVals *initvals) {
	MemRd &ಪೋರ್ಟ್ = rd_ports[idx];

	ಆದರೆ (!ಪೋರ್ಟ್.clk_enable)
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

	log_assert(module && module->design);
	std::string memid_str = memid.str();

	Cell *c;

	// There are two ways to handle rdff extraction when transparency is involved:
	//
	// - if all of the following conditions are true, put the FF on address input:
	//
	//   - the port has no clock enable, no reset, and no initial value
	//   - the port is transparent wrt all write ports (implying they also share
	//     the clock domain)
	//
	// - otherwise, put the FF on the data output, and make bypass paths for
	//   all write ports wrt which this port is transparent
	ತಾರ್ಕಿಕ trans_use_addr = ನಿಜ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++)
		ಆದರೆ (!ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[i] && !wr_ports[i].removed)
			trans_use_addr = ಸುಳ್ಳು;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	ಆದರೆ (Get_ಗಾತ್ರ(wr_ports) == 0)
		trans_use_addr = ಸುಳ್ಳು;

	ಆದರೆ (ಪೋರ್ಟ್.en != State::S1 || ಪೋರ್ಟ್.srst != State::S0 || ಪೋರ್ಟ್.arst != State::S0 || !ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.is_fully_undef())
		trans_use_addr = ಸುಳ್ಳು;

	ಆದರೆ (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr); i++)
			ಆದರೆ (ಪೋರ್ಟ್.addr[i].wire)
				ಅಗಲ++;

		ಆದರೆ (ಅಗಲ)
		{
			SigSpec sig_q = module->ಕೂಡಿಸು_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), ಅಗಲ);
			SigSpec sig_d;

			ಪೂರ್ಣಾಂಕ pos = 0;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr); i++)
				ಆದರೆ (ಪೋರ್ಟ್.addr[i].wire) {
					sig_d.append(ಪೋರ್ಟ್.addr[i]);
					ಪೋರ್ಟ್.addr[i] = sig_q[pos++];
				}

			c = module->ಕೂಡಿಸು_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), ಪೋರ್ಟ್.clk, sig_d, sig_q, ಪೋರ್ಟ್.clk_polarity);
		} ಇಲ್ಲದಿದ್ದರೆ {
			c = ಶೂನ್ಯಸೂಚಕ;
		}
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		log_assert(ಪೋರ್ಟ್.arst == State::S0 || ಪೋರ್ಟ್.srst == State::S0);

		SigSpec async_d = module->ಕೂಡಿಸು_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
		SigSpec sig_d = async_d;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
			ಸ್ವಯಂಚಾಲಿತ &wport = wr_ports[i];
			ಆದರೆ (wport.removed)
				ಮುಂದುವರಿ;
			ಆದರೆ (ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[i] || ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == ಪೋರ್ಟ್.clk);
				log_assert(wport.clk_enable == ಪೋರ್ಟ್.clk_enable);
				ಪೂರ್ಣಾಂಕ min_wide_log2 = std::min(ಪೋರ್ಟ್.wide_log2, wport.wide_log2);
				ಪೂರ್ಣಾಂಕ max_wide_log2 = std::max(ಪೋರ್ಟ್.wide_log2, wport.wide_log2);
				ತಾರ್ಕಿಕ wide_write = wport.wide_log2 > ಪೋರ್ಟ್.wide_log2;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = ಪೋರ್ಟ್.addr;
					SigSpec waddr = wport.addr;
					ಆದರೆ (wide_write)
						waddr = wport.sub_addr(sub);
					ಇಲ್ಲದಿದ್ದರೆ
						raddr = ಪೋರ್ಟ್.sub_addr(sub);
					SigSpec addr_eq;
					ಆದರೆ (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					ಪೂರ್ಣಾಂಕ pos = 0;
					ಪೂರ್ಣಾಂಕ ewidth = ಅಗಲ << min_wide_log2;
					ಪೂರ್ಣಾಂಕ wsub = wide_write ? sub : 0;
					ಪೂರ್ಣಾಂಕ rsub = wide_write ? 0 : sub;
					ಆಗ (pos < ewidth) {
						ಪೂರ್ಣಾಂಕ epos = pos;
						ಆಗ (epos < ewidth && wport.en[epos + wsub * ಅಗಲ] == wport.en[pos + wsub * ಅಗಲ])
							epos++;
						SigSpec cur = sig_d.ಹೊರತೆಗೆ(pos + rsub * ಅಗಲ, epos-pos);
						SigSpec other = ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[i] ? wport.ದತ್ತಾಂಶ.ಹೊರತೆಗೆ(pos + wsub * ಅಗಲ, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						ಆದರೆ (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * ಅಗಲ], addr_eq);
						ಇಲ್ಲದಿದ್ದರೆ
							cond = wport.en[pos + wsub * ಅಗಲ];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * ಅಗಲ, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().ಕೂಡಿಸು(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_ದತ್ತಾಂಶ ff(module, initvals, name);
		ff.ಅಗಲ = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		ff.has_clk = ನಿಜ;
		ff.sig_clk = ಪೋರ್ಟ್.clk;
		ff.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ಆದರೆ (ಪೋರ್ಟ್.en != State::S1) {
			ff.has_ce = ನಿಜ;
			ff.pol_ce = ನಿಜ;
			ff.sig_ce = ಪೋರ್ಟ್.en;
		}
		ಆದರೆ (ಪೋರ್ಟ್.arst != State::S0) {
			ff.has_arst = ನಿಜ;
			ff.pol_arst = ನಿಜ;
			ff.sig_arst = ಪೋರ್ಟ್.arst;
			ff.val_arst = ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ;
		}
		ಆದರೆ (ಪೋರ್ಟ್.srst != State::S0) {
			ff.has_srst = ನಿಜ;
			ff.pol_srst = ನಿಜ;
			ff.sig_srst = ಪೋರ್ಟ್.srst;
			ff.val_srst = ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ;
			ff.ce_over_srst = ff.has_ce && ಪೋರ್ಟ್.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = ಪೋರ್ಟ್.ದತ್ತಾಂಶ;
		ff.val_init = ಪೋರ್ಟ್.init_ಮೌಲ್ಯ;
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = async_d;
		c = ff.emit();
	}

	ಆದರೆ (c)
		ಲಘುಗಣಕ("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	ಪೋರ್ಟ್.en = State::S1;
	ಪೋರ್ಟ್.clk = State::S0;
	ಪೋರ್ಟ್.arst = State::S0;
	ಪೋರ್ಟ್.srst = State::S0;
	ಪೋರ್ಟ್.clk_enable = ಸುಳ್ಳು;
	ಪೋರ್ಟ್.clk_polarity = ನಿಜ;
	ಪೋರ್ಟ್.ce_over_srst = ಸುಳ್ಳು;
	ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	ಪೋರ್ಟ್.init_ಮೌಲ್ಯ = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
		ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ c;
}

ಶೂನ್ಯ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::ಸದಿಶ<MemRd> new_rd_ports;
	std::ಸದಿಶ<MemWr> new_wr_ports;
	std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> new_rd_map;
	std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> new_wr_map;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[i];
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << ಪೋರ್ಟ್.wide_log2); sub++) {
			new_rd_map.ತಳ್ಳು_ಹಿಂಭಾಗ(std::make_pair(i, sub));
		}
	}
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = wr_ports[i];
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << ಪೋರ್ಟ್.wide_log2); sub++) {
			new_wr_map.ತಳ್ಳು_ಹಿಂಭಾಗ(std::make_pair(i, sub));
		}
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : new_rd_map) {
		MemRd &orig = rd_ports[it.ಮೊದಲ];
		MemRd ಪೋರ್ಟ್ = orig;
		ಆದರೆ (it.ಎರಡನೇ != 0)
			ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
		ಆದರೆ (ಪೋರ್ಟ್.wide_log2) {
			ಪೋರ್ಟ್.ದತ್ತಾಂಶ = ಪೋರ್ಟ್.ದತ್ತಾಂಶ.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.init_ಮೌಲ್ಯ = ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ = ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ = ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.addr = ಪೋರ್ಟ್.sub_addr(it.ಎರಡನೇ);
			ಪೋರ್ಟ್.wide_log2 = 0;
		}
		ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.clear();
		ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.clear();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it2 : new_wr_map)
			ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(orig.transparency_ಮುಖವಾಡ[it2.ಮೊದಲ]);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it2 : new_wr_map)
			ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(orig.collision_x_ಮುಖವಾಡ[it2.ಮೊದಲ]);
		new_rd_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : new_wr_map) {
		MemWr &orig = wr_ports[it.ಮೊದಲ];
		MemWr ಪೋರ್ಟ್ = orig;
		ಆದರೆ (it.ಎರಡನೇ != 0)
			ಪೋರ್ಟ್.cell = ಶೂನ್ಯಸೂಚಕ;
		ಆದರೆ (ಪೋರ್ಟ್.wide_log2) {
			ಪೋರ್ಟ್.ದತ್ತಾಂಶ = ಪೋರ್ಟ್.ದತ್ತಾಂಶ.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.en = ಪೋರ್ಟ್.en.ಹೊರತೆಗೆ(it.ಎರಡನೇ * ಅಗಲ, ಅಗಲ);
			ಪೋರ್ಟ್.addr = ಪೋರ್ಟ್.sub_addr(it.ಎರಡನೇ);
			ಪೋರ್ಟ್.wide_log2 = 0;
		}
		ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.clear();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it2 : new_wr_map)
			ಪೋರ್ಟ್.ಆದ್ಯತೆ_ಮುಖವಾಡ.ತಳ್ಳು_ಹಿಂಭಾಗ(orig.ಆದ್ಯತೆ_ಮುಖವಾಡ[it2.ಮೊದಲ]);
		new_wr_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
	}
	std::ಅದಲುಬದಲು(rd_ports, new_rd_ports);
	std::ಅದಲುಬದಲು(wr_ports, new_wr_ports);
}

ಶೂನ್ಯ Mem::emulate_ಆದ್ಯತೆ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals)
{
	ಸ್ವಯಂಚಾಲಿತ &port1 = wr_ports[idx1];
	ಸ್ವಯಂಚಾಲಿತ &port2 = wr_ports[idx2];
	ಆದರೆ (!port2.ಆದ್ಯತೆ_ಮುಖವಾಡ[idx1])
		ಹಿಂತಿರುಗಿ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &rport = rd_ports[i];
		ಆದರೆ (rport.removed)
			ಮುಂದುವರಿ;
		ಆದರೆ (rport.transparency_ಮುಖವಾಡ[idx1] && !(rport.transparency_ಮುಖವಾಡ[idx2] || rport.collision_x_ಮುಖವಾಡ[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	ಪೂರ್ಣಾಂಕ min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	ಪೂರ್ಣಾಂಕ max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	ತಾರ್ಕಿಕ wide1 = port1.wide_log2 > port2.wide_log2;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		ಆದರೆ (wide1)
			addr1 = port1.sub_addr(sub);
		ಇಲ್ಲದಿದ್ದರೆ
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		ಪೂರ್ಣಾಂಕ ewidth = ಅಗಲ << min_wide_log2;
		ಪೂರ್ಣಾಂಕ sub1 = wide1 ? sub : 0;
		ಪೂರ್ಣಾಂಕ sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್>, Sig_ಬಿಟ್> cache;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pos = 0; pos < ewidth; pos++) {
			Sig_ಬಿಟ್ &en1 = port1.en[pos + sub1 * ಅಗಲ];
			Sig_ಬಿಟ್ &en2 = port2.en[pos + sub2 * ಅಗಲ];
			std::pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್> ಕೀಲಿ(en1, en2);
			ಆದರೆ (cache.ಎಣಿಕೆ_2(ಕೀಲಿ)) {
				en1 = cache[ಕೀಲಿ];
			} ಇಲ್ಲದಿದ್ದರೆ {
				Sig_ಬಿಟ್ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_ಬಿಟ್ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[ಕೀಲಿ] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ಆದ್ಯತೆ_ಮುಖವಾಡ[idx1] = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Mem::emulate_transparency(ಪೂರ್ಣಾಂಕ widx, ಪೂರ್ಣಾಂಕ ridx, FfInitVals *initvals) {
	ಸ್ವಯಂಚಾಲಿತ &wport = wr_ports[widx];
	ಸ್ವಯಂಚಾಲಿತ &rport = rd_ports[ridx];
	log_assert(rport.transparency_ಮುಖವಾಡ[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(wr_ports) - 1; i > widx; i--) {
		ಆದರೆ (wr_ports[i].removed)
			ಮುಂದುವರಿ;
		ಆದರೆ (rport.transparency_ಮುಖವಾಡ[i] && wr_ports[i].ಆದ್ಯತೆ_ಮುಖವಾಡ[widx])
			emulate_transparency(i, ridx, initvals);
	}
	ಪೂರ್ಣಾಂಕ min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	ಪೂರ್ಣಾಂಕ max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	ತಾರ್ಕಿಕ wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(wport.ದತ್ತಾಂಶ));
	module->ಕೂಡಿಸು_Dffe(NEW_ID, rport.clk, rport.en, wport.ದತ್ತಾಂಶ, wdata_q, rport.clk_polarity, ನಿಜ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = min_wide_log2; j < max_wide_log2; j++)
			ಆದರೆ (wide_write)
				waddr = wport.sub_addr(sub);
			ಇಲ್ಲದಿದ್ದರೆ
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		ಆದರೆ (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		ಪೂರ್ಣಾಂಕ pos = 0;
		ಪೂರ್ಣಾಂಕ ewidth = ಅಗಲ << min_wide_log2;
		ಪೂರ್ಣಾಂಕ wsub = wide_write ? sub : 0;
		ಪೂರ್ಣಾಂಕ rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->ಕೂಡಿಸು_Wire(NEW_ID, ewidth);
		ಆಗ (pos < ewidth) {
			ಪೂರ್ಣಾಂಕ epos = pos;
			ಆಗ (epos < ewidth && wport.en[epos + wsub * ಅಗಲ] == wport.en[pos + wsub * ಅಗಲ])
				epos++;
			SigSpec cond;
			ಆದರೆ (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * ಅಗಲ], addr_eq);
			ಇಲ್ಲದಿದ್ದರೆ
				cond = wport.en[pos + wsub * ಅಗಲ];
			SigSpec cond_q = module->ಕೂಡಿಸು_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_ದತ್ತಾಂಶ ff(module, initvals, NEW_ID);
			ff.ಅಗಲ = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = ನಿಜ;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			ಆದರೆ (rport.en != State::S1) {
				ff.has_ce = ನಿಜ;
				ff.sig_ce = rport.en;
				ff.pol_ce = ನಿಜ;
			}
			ಆದರೆ (rport.arst != State::S0) {
				ff.has_arst = ನಿಜ;
				ff.sig_arst = rport.arst;
				ff.pol_arst = ನಿಜ;
				ff.val_arst = State::S0;
			}
			ಆದರೆ (rport.srst != State::S0) {
				ff.has_srst = ನಿಜ;
				ff.sig_srst = rport.srst;
				ff.pol_srst = ನಿಜ;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			ಆದರೆ (!rport.init_ಮೌಲ್ಯ.is_fully_undef())
				ff.val_init = State::S0;
			ಇಲ್ಲದಿದ್ದರೆ
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.ಹೊರತೆಗೆ(pos, epos-pos);
			SigSpec other = wdata_q.ಹೊರತೆಗೆ(pos + wsub * ಅಗಲ, epos-pos);
			SigSpec dest = rport.ದತ್ತಾಂಶ.ಹೊರತೆಗೆ(pos + rsub * ಅಗಲ, epos-pos);
			module->ಕೂಡಿಸು_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.ದತ್ತಾಂಶ.replace(rsub * ಅಗಲ, rdata_a);
	}
	rport.transparency_ಮುಖವಾಡ[widx] = ಸುಳ್ಳು;
	rport.collision_x_ಮುಖವಾಡ[widx] = ನಿಜ;
}

ಶೂನ್ಯ Mem::prepare_wr_ವಿಲೀನ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	ಸ್ವಯಂಚಾಲಿತ &port1 = wr_ports[idx1];
	ಸ್ವಯಂಚಾಲಿತ &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < idx1; i++)
		ಆದರೆ (port2.ಆದ್ಯತೆ_ಮುಖವಾಡ[i])
			port1.ಆದ್ಯತೆ_ಮುಖವಾಡ[i] = ನಿಜ;
	// If port 2 has priority over a port after port 1, emulate it.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = idx1 + 1; i < idx2; i++)
		ಆದರೆ (port2.ಆದ್ಯತೆ_ಮುಖವಾಡ[i] && !wr_ports[i].removed)
			emulate_ಆದ್ಯತೆ(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = idx2 + 1; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &oport = wr_ports[i];
		ಆದರೆ (oport.ಆದ್ಯತೆ_ಮುಖವಾಡ[idx2])
			oport.ಆದ್ಯತೆ_ಮುಖವಾಡ[idx1] = ನಿಜ;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++) {
		ಸ್ವಯಂಚಾಲಿತ &rport = rd_ports[i];
		ಆದರೆ (rport.removed)
			ಮುಂದುವರಿ;
		// If collision already undefined with both ports, it's fine.
		ಆದರೆ (rport.collision_x_ಮುಖವಾಡ[idx1] && rport.collision_x_ಮುಖವಾಡ[idx2])
			ಮುಂದುವರಿ;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		ಆದರೆ (rport.collision_x_ಮುಖವಾಡ[idx1]) {
			rport.collision_x_ಮುಖವಾಡ[idx1] = ಸುಳ್ಳು;
			rport.transparency_ಮುಖವಾಡ[idx1] = rport.transparency_ಮುಖವಾಡ[idx2];
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (rport.collision_x_ಮುಖವಾಡ[idx2]) {
			rport.collision_x_ಮುಖವಾಡ[idx2] = ಸುಳ್ಳು;
			rport.transparency_ಮುಖವಾಡ[idx2] = rport.transparency_ಮುಖವಾಡ[idx1];
			ಮುಂದುವರಿ;
		}
		// If transparent with both ports, also fine.
		ಆದರೆ (rport.transparency_ಮುಖವಾಡ[idx1] && rport.transparency_ಮುಖವಾಡ[idx2])
			ಮುಂದುವರಿ;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		ಆದರೆ (rport.transparency_ಮುಖವಾಡ[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_ಮುಖವಾಡ[idx1] = ಸುಳ್ಳು;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (rport.transparency_ಮುಖವಾಡ[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_ಮುಖವಾಡ[idx2] = ಸುಳ್ಳು;
			ಮುಂದುವರಿ;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

ಶೂನ್ಯ Mem::prepare_rd_ವಿಲೀನ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals) {
	ಸ್ವಯಂಚಾಲಿತ &port1 = rd_ports[idx1];
	ಸ್ವಯಂಚಾಲಿತ &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wr_ports); i++) {
		ಆದರೆ (wr_ports[i].removed)
			ಮುಂದುವರಿ;
		// Both ports undefined, OK.
		ಆದರೆ (port1.collision_x_ಮುಖವಾಡ[i] && port2.collision_x_ಮುಖವಾಡ[i])
			ಮುಂದುವರಿ;
		// Only one port undefined — change its behavior
		// to align with the other port.
		ಆದರೆ (port1.collision_x_ಮುಖವಾಡ[i]) {
			port1.collision_x_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
			port1.transparency_ಮುಖವಾಡ[i] = port2.transparency_ಮುಖವಾಡ[i];
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (port2.collision_x_ಮುಖವಾಡ[i]) {
			port2.collision_x_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
			port2.transparency_ಮುಖವಾಡ[i] = port1.transparency_ಮುಖವಾಡ[i];
			ಮುಂದುವರಿ;
		}
		// Both ports transparent, OK.
		ಆದರೆ (port1.transparency_ಮುಖವಾಡ[i] && port2.transparency_ಮುಖವಾಡ[i])
			ಮುಂದುವರಿ;
		// Only one port transparent — emulate transparency
		// on the other.
		ಆದರೆ (port1.transparency_ಮುಖವಾಡ[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (port2.transparency_ಮುಖವಾಡ[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_ಮುಖವಾಡ[i] = ಸುಳ್ಳು;
			ಮುಂದುವರಿ;
		}
		// No ports transparent, OK.
	}

}

ಶೂನ್ಯ Mem::widen_prep(ಪೂರ್ಣಾಂಕ wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ = ((1 << wide_log2) - 1);
	ಪೂರ್ಣಾಂಕ delta = ಆರಂಭ_ಸ್ಥಾನಾಂತರ & ಮುಖವಾಡ;
	ಆರಂಭ_ಸ್ಥಾನಾಂತರ -= delta;
	ಗಾತ್ರ_2 += delta;
	ಆದರೆ (ಗಾತ್ರ_2 & ಮುಖವಾಡ) {
		ಗಾತ್ರ_2 |= ಮುಖವಾಡ;
		ಗಾತ್ರ_2++;
	}
}

ಶೂನ್ಯ Mem::widen_wr_ಪೋರ್ಟ್(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ wide_log2) {
	widen_prep(wide_log2);
	ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = wr_ports[idx];
	log_assert(ಪೋರ್ಟ್.wide_log2 <= wide_log2);
	ಆದರೆ (ಪೋರ್ಟ್.wide_log2 < wide_log2) {
		SigSpec new_ದತ್ತಾಂಶ, new_en;
		SigSpec addr_lo = ಪೋರ್ಟ್.addr.ಹೊರತೆಗೆ(0, wide_log2);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sub = 0; sub < (1 << wide_log2); sub += (1 << ಪೋರ್ಟ್.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			ಆದರೆ (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_ದತ್ತಾಂಶ.append(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
				new_en.append(ಪೋರ್ಟ್.en);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_ದತ್ತಾಂಶ.append(Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ)));
				new_en.append(Const(State::S0, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ)));
			} ಇಲ್ಲದಿದ್ದರೆ {
				// May or may not write to this subword.
				new_ದತ್ತಾಂಶ.append(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ)), ಪೋರ್ಟ್.en, addr_eq);
				new_en.append(en);
			}
		}
		ಪೋರ್ಟ್.addr.replace(ಪೋರ್ಟ್.wide_log2, Const(State::S0, wide_log2 - ಪೋರ್ಟ್.wide_log2));
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
		ಪೋರ್ಟ್.en = new_en;
		ಪೋರ್ಟ್.wide_log2 = wide_log2;
	}
}

ಶೂನ್ಯ Mem::emulate_rden(ಪೂರ್ಣಾಂಕ idx, FfInitVals *initvals) {
	ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[idx];
	log_assert(ಪೋರ್ಟ್.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	Wire *prev_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	Wire *sel = module->ಕೂಡಿಸು_Wire(NEW_ID);
	Ff_ದತ್ತಾಂಶ ff_sel(module, initvals, NEW_ID);
	Ff_ದತ್ತಾಂಶ ff_ದತ್ತಾಂಶ(module, initvals, NEW_ID);
	ff_sel.ಅಗಲ = 1;
	ff_sel.has_clk = ನಿಜ;
	ff_sel.sig_clk = ಪೋರ್ಟ್.clk;
	ff_sel.pol_clk = ಪೋರ್ಟ್.clk_polarity;
	ff_sel.sig_d = ಪೋರ್ಟ್.en;
	ff_sel.sig_q = sel;
	ff_ದತ್ತಾಂಶ.ಅಗಲ = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
	ff_ದತ್ತಾಂಶ.has_clk = ನಿಜ;
	ff_ದತ್ತಾಂಶ.sig_clk = ಪೋರ್ಟ್.clk;
	ff_ದತ್ತಾಂಶ.pol_clk = ಪೋರ್ಟ್.clk_polarity;
	ff_ದತ್ತಾಂಶ.sig_d = ಪೋರ್ಟ್.ದತ್ತಾಂಶ;
	ff_ದತ್ತಾಂಶ.sig_q = prev_ದತ್ತಾಂಶ;
	ಆದರೆ (!ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_ದತ್ತಾಂಶ.val_init = ಪೋರ್ಟ್.init_ಮೌಲ್ಯ;
		ಪೋರ್ಟ್.init_ಮೌಲ್ಯ = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	} ಇಲ್ಲದಿದ್ದರೆ {
		ff_sel.val_init = State::Sx;
		ff_ದತ್ತಾಂಶ.val_init = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	}
	ಆದರೆ (ಪೋರ್ಟ್.arst != State::S0) {
		ff_sel.has_arst = ನಿಜ;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = ಪೋರ್ಟ್.arst;
		ff_sel.pol_arst = ನಿಜ;
		ff_ದತ್ತಾಂಶ.has_arst = ನಿಜ;
		ff_ದತ್ತಾಂಶ.val_arst = ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ;
		ff_ದತ್ತಾಂಶ.sig_arst = ಪೋರ್ಟ್.arst;
		ff_ದತ್ತಾಂಶ.pol_arst = ನಿಜ;
		ಪೋರ್ಟ್.arst = State::S0;
	}
	ಆದರೆ (ಪೋರ್ಟ್.srst != State::S0) {
		log_assert(!ಪೋರ್ಟ್.ce_over_srst);
		ff_sel.has_srst = ನಿಜ;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = ಪೋರ್ಟ್.srst;
		ff_sel.pol_srst = ನಿಜ;
		ff_sel.ce_over_srst = ಸುಳ್ಳು;
		ff_ದತ್ತಾಂಶ.has_srst = ನಿಜ;
		ff_ದತ್ತಾಂಶ.val_srst = ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ;
		ff_ದತ್ತಾಂಶ.sig_srst = ಪೋರ್ಟ್.srst;
		ff_ದತ್ತಾಂಶ.pol_srst = ನಿಜ;
		ff_ದತ್ತಾಂಶ.ce_over_srst = ಸುಳ್ಳು;
		ಪೋರ್ಟ್.srst = State::S0;
	}
	ff_sel.emit();
	ff_ದತ್ತಾಂಶ.emit();
	module->ಕೂಡಿಸು_Mux(NEW_ID, prev_ದತ್ತಾಂಶ, new_ದತ್ತಾಂಶ, sel, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
	ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
	ಪೋರ್ಟ್.en = State::S1;
}

ಶೂನ್ಯ Mem::emulate_reset(ಪೂರ್ಣಾಂಕ idx, ತಾರ್ಕಿಕ emu_init, ತಾರ್ಕಿಕ emu_arst, ತಾರ್ಕಿಕ emu_srst, FfInitVals *initvals) {
	ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[idx];
	ಆದರೆ (emu_init && !ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.is_fully_undef()) {
		Wire *sel = module->ಕೂಡಿಸು_Wire(NEW_ID);
		Ff_ದತ್ತಾಂಶ ff_sel(module, initvals, NEW_ID);
		Wire *new_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
		ff_sel.ಅಗಲ = 1;
		ff_sel.has_clk = ನಿಜ;
		ff_sel.sig_clk = ಪೋರ್ಟ್.clk;
		ff_sel.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		ಆದರೆ (ಪೋರ್ಟ್.en != State::S1) {
			ff_sel.has_ce = ನಿಜ;
			ff_sel.sig_ce = ಪೋರ್ಟ್.en;
			ff_sel.pol_ce = ನಿಜ;
			ff_sel.ce_over_srst = ಪೋರ್ಟ್.ce_over_srst;
		}
		ಆದರೆ (ಪೋರ್ಟ್.arst != State::S0) {
			ff_sel.has_arst = ನಿಜ;
			ff_sel.sig_arst = ಪೋರ್ಟ್.arst;
			ff_sel.pol_arst = ನಿಜ;
			ಆದರೆ (emu_arst && ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ == ಪೋರ್ಟ್.init_ಮೌಲ್ಯ) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				ಪೋರ್ಟ್.arst = State::S0;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ff_sel.val_arst = State::S1;
			}
		}
		ಆದರೆ (ಪೋರ್ಟ್.srst != State::S0) {
			ff_sel.has_srst = ನಿಜ;
			ff_sel.sig_srst = ಪೋರ್ಟ್.srst;
			ff_sel.pol_srst = ನಿಜ;
			ಆದರೆ (emu_srst && ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ == ಪೋರ್ಟ್.init_ಮೌಲ್ಯ) {
				ff_sel.val_srst = State::S0;
				ಪೋರ್ಟ್.srst = State::S0;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->ಕೂಡಿಸು_Mux(NEW_ID, ಪೋರ್ಟ್.init_ಮೌಲ್ಯ, new_ದತ್ತಾಂಶ, sel, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
		ಪೋರ್ಟ್.init_ಮೌಲ್ಯ = Const(State::Sx, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
	}
	ಆದರೆ (emu_arst && ಪೋರ್ಟ್.arst != State::S0) {
		Wire *sel = module->ಕೂಡಿಸು_Wire(NEW_ID);
		Ff_ದತ್ತಾಂಶ ff_sel(module, initvals, NEW_ID);
		Wire *new_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
		ff_sel.ಅಗಲ = 1;
		ff_sel.has_clk = ನಿಜ;
		ff_sel.sig_clk = ಪೋರ್ಟ್.clk;
		ff_sel.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ಆದರೆ (ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ಇಲ್ಲದಿದ್ದರೆ
			ff_sel.val_init = State::S1;
		ಆದರೆ (ಪೋರ್ಟ್.en != State::S1) {
			ff_sel.has_ce = ನಿಜ;
			ff_sel.sig_ce = ಪೋರ್ಟ್.en;
			ff_sel.pol_ce = ನಿಜ;
			ff_sel.ce_over_srst = ಪೋರ್ಟ್.ce_over_srst;
		}
		ff_sel.has_arst = ನಿಜ;
		ff_sel.sig_arst = ಪೋರ್ಟ್.arst;
		ff_sel.pol_arst = ನಿಜ;
		ff_sel.val_arst = State::S0;
		ಆದರೆ (ಪೋರ್ಟ್.srst != State::S0) {
			ff_sel.has_srst = ನಿಜ;
			ff_sel.sig_srst = ಪೋರ್ಟ್.srst;
			ff_sel.pol_srst = ನಿಜ;
			ಆದರೆ (emu_srst && ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ == ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ) {
				ff_sel.val_srst = State::S0;
				ಪೋರ್ಟ್.srst = State::S0;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->ಕೂಡಿಸು_Mux(NEW_ID, ಪೋರ್ಟ್.arst_ಮೌಲ್ಯ, new_ದತ್ತಾಂಶ, sel, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
		ಪೋರ್ಟ್.arst = State::S0;
	}
	ಆದರೆ (emu_srst && ಪೋರ್ಟ್.srst != State::S0) {
		Wire *sel = module->ಕೂಡಿಸು_Wire(NEW_ID);
		Ff_ದತ್ತಾಂಶ ff_sel(module, initvals, NEW_ID);
		Wire *new_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
		ff_sel.ಅಗಲ = 1;
		ff_sel.has_clk = ನಿಜ;
		ff_sel.sig_clk = ಪೋರ್ಟ್.clk;
		ff_sel.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ಆದರೆ (ಪೋರ್ಟ್.init_ಮೌಲ್ಯ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ಇಲ್ಲದಿದ್ದರೆ
			ff_sel.val_init = State::S1;
		ಆದರೆ (ಪೋರ್ಟ್.en != State::S1) {
			ff_sel.has_ce = ನಿಜ;
			ff_sel.sig_ce = ಪೋರ್ಟ್.en;
			ff_sel.pol_ce = ನಿಜ;
			ff_sel.ce_over_srst = ಪೋರ್ಟ್.ce_over_srst;
		}
		ff_sel.has_srst = ನಿಜ;
		ff_sel.sig_srst = ಪೋರ್ಟ್.srst;
		ff_sel.pol_srst = ನಿಜ;
		ff_sel.val_srst = State::S0;
		ಆದರೆ (ಪೋರ್ಟ್.arst != State::S0) {
			ff_sel.has_arst = ನಿಜ;
			ff_sel.sig_arst = ಪೋರ್ಟ್.arst;
			ff_sel.pol_arst = ನಿಜ;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->ಕೂಡಿಸು_Mux(NEW_ID, ಪೋರ್ಟ್.srst_ಮೌಲ್ಯ, new_ದತ್ತಾಂಶ, sel, ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
		ಪೋರ್ಟ್.srst = State::S0;
	}
}

ಶೂನ್ಯ Mem::emulate_rd_ce_over_srst(ಪೂರ್ಣಾಂಕ idx) {
	ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[idx];
	log_assert(ಪೋರ್ಟ್.clk_enable);
	ಆದರೆ (ಪೋರ್ಟ್.en == State::S1 || ಪೋರ್ಟ್.srst == State::S0 || !ಪೋರ್ಟ್.ce_over_srst) {
		ಪೋರ್ಟ್.ce_over_srst = ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ;
	}
	ಪೋರ್ಟ್.ce_over_srst = ಸುಳ್ಳು;
	ಪೋರ್ಟ್.srst = module->And(NEW_ID, ಪೋರ್ಟ್.en, ಪೋರ್ಟ್.srst);
}

ಶೂನ್ಯ Mem::emulate_rd_srst_over_ce(ಪೂರ್ಣಾಂಕ idx) {
	ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = rd_ports[idx];
	log_assert(ಪೋರ್ಟ್.clk_enable);
	ಆದರೆ (ಪೋರ್ಟ್.en == State::S1 || ಪೋರ್ಟ್.srst == State::S0 || ಪೋರ್ಟ್.ce_over_srst) {
		ಪೋರ್ಟ್.ce_over_srst = ನಿಜ;
		ಹಿಂತಿರುಗಿ;
	}
	ಪೋರ್ಟ್.ce_over_srst = ನಿಜ;
	ಪೋರ್ಟ್.en = module->Or(NEW_ID, ಪೋರ್ಟ್.en, ಪೋರ್ಟ್.srst);
}

ತಾರ್ಕಿಕ Mem::emulate_read_ಮೊದಲ_ok() {
	ಆದರೆ (wr_ports.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	SigSpec clk = wr_ports[0].clk;
	ತಾರ್ಕಿಕ clk_polarity = wr_ports[0].clk_polarity;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್: wr_ports) {
		ಆದರೆ (!ಪೋರ್ಟ್.clk_enable)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (ಪೋರ್ಟ್.clk != clk)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (ಪೋರ್ಟ್.clk_polarity != clk_polarity)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು_read_ಮೊದಲ = ಸುಳ್ಳು;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್: rd_ports) {
		ಆದರೆ (!ಪೋರ್ಟ್.clk_enable)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (ಪೋರ್ಟ್.clk != clk)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (ಪೋರ್ಟ್.clk_polarity != clk_polarity)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(wr_ports); j++)
			ಆದರೆ (!ಪೋರ್ಟ್.transparency_ಮುಖವಾಡ[j] && !ಪೋರ್ಟ್.collision_x_ಮುಖವಾಡ[j])
				ಸಿಕ್ಕಿತು_read_ಮೊದಲ = ನಿಜ;
	}
	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_read_ಮೊದಲ;
}

ಶೂನ್ಯ Mem::emulate_read_ಮೊದಲ(FfInitVals *initvals) {
	log_assert(emulate_read_ಮೊದಲ_ok());
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(wr_ports); j++)
			ಆದರೆ (rd_ports[i].transparency_ಮುಖವಾಡ[j])
				emulate_transparency(j, i, initvals);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(rd_ports); i++)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_ಮುಖವಾಡ[j]);
			rd_ports[i].collision_x_ಮುಖವಾಡ[j] = ಸುಳ್ಳು;
			rd_ports[i].transparency_ಮುಖವಾಡ[j] = ನಿಜ;
		}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್: wr_ports) {
		Wire *new_ದತ್ತಾಂಶ = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ));
		Wire *new_addr = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr));
		ಸ್ವಯಂಚಾಲಿತ compressed = ಪೋರ್ಟ್.compress_en();
		Wire *new_en = module->ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(compressed.ಮೊದಲ));
		Ff_ದತ್ತಾಂಶ ff_ದತ್ತಾಂಶ(module, initvals, NEW_ID);
		Ff_ದತ್ತಾಂಶ ff_addr(module, initvals, NEW_ID);
		Ff_ದತ್ತಾಂಶ ff_en(module, initvals, NEW_ID);
		ff_ದತ್ತಾಂಶ.ಅಗಲ = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ದತ್ತಾಂಶ);
		ff_ದತ್ತಾಂಶ.has_clk = ನಿಜ;
		ff_ದತ್ತಾಂಶ.sig_clk = ಪೋರ್ಟ್.clk;
		ff_ದತ್ತಾಂಶ.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_ದತ್ತಾಂಶ.sig_d = ಪೋರ್ಟ್.ದತ್ತಾಂಶ;
		ff_ದತ್ತಾಂಶ.sig_q = new_ದತ್ತಾಂಶ;;
		ff_ದತ್ತಾಂಶ.val_init = Const(State::Sx, ff_ದತ್ತಾಂಶ.ಅಗಲ);
		ff_ದತ್ತಾಂಶ.emit();
		ff_addr.ಅಗಲ = Get_ಗಾತ್ರ(ಪೋರ್ಟ್.addr);
		ff_addr.has_clk = ನಿಜ;
		ff_addr.sig_clk = ಪೋರ್ಟ್.clk;
		ff_addr.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_addr.sig_d = ಪೋರ್ಟ್.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.ಅಗಲ);
		ff_addr.emit();
		ff_en.ಅಗಲ = Get_ಗಾತ್ರ(compressed.ಮೊದಲ);
		ff_en.has_clk = ನಿಜ;
		ff_en.sig_clk = ಪೋರ್ಟ್.clk;
		ff_en.pol_clk = ಪೋರ್ಟ್.clk_polarity;
		ff_en.sig_d = compressed.ಮೊದಲ;
		ff_en.sig_q = new_en;;
		ಆದರೆ (inits.ಖಾಲಿಗಳು())
			ff_en.val_init = Const(State::Sx, ff_en.ಅಗಲ);
		ಇಲ್ಲದಿದ್ದರೆ
			ff_en.val_init = Const(State::S0, ff_en.ಅಗಲ);
		ff_en.emit();
		ಪೋರ್ಟ್.ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
		ಪೋರ್ಟ್.addr = new_addr;
		ಪೋರ್ಟ್.en = ಪೋರ್ಟ್.decompress_en(compressed.ಎರಡನೇ, new_en);
	}
}

std::pair<SigSpec, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> swizzle;
	Sig_ಬಿಟ್ prev_ಬಿಟ್ = en[0];
	ಪೂರ್ಣಾಂಕ idx = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್: en) {
		ಆದರೆ (ಬಿಟ್ != prev_ಬಿಟ್) {
			sig.append(ಬಿಟ್);
			prev_ಬಿಟ್ = ಬಿಟ್;
			idx++;
		}
		swizzle.ತಳ್ಳು_ಹಿಂಭಾಗ(idx);
	}
	log_assert(idx + 1 == Get_ಗಾತ್ರ(sig));
	ಹಿಂತಿರುಗಿ {sig, swizzle};
}

SigSpec MemWr::decompress_en(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &swizzle, SigSpec sig) {
	SigSpec res;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i: swizzle)
		res.append(sig[i]);
	ಹಿಂತಿರುಗಿ res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->ಗಾತ್ರ_2), mem->ಅಗಲ)
{
	ಗಾಗಿ(ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &init : mem->inits) {
		ಆದರೆ(init.en.is_fully_zero()) ಮುಂದುವರಿ;
		log_assert(init.en.ಗಾತ್ರ_2() == ದತ್ತಾಂಶ_ಅಗಲ_2);
		ಆದರೆ(init.en.is_fully_ones())
			ಸೇರಿಸು_concatenated(init.addr.as_int(), init.ದತ್ತಾಂಶ);
		ಇಲ್ಲದಿದ್ದರೆ {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t ಪದಗಳು_2 = init.ದತ್ತಾಂಶ.ಗಾತ್ರ_2() / ದತ್ತಾಂಶ_ಅಗಲ_2;
			RTLIL::Const ದತ್ತಾಂಶ = init.ದತ್ತಾಂಶ;
			log_assert(ದತ್ತಾಂಶ.ಗಾತ್ರ_2() % ದತ್ತಾಂಶ_ಅಗಲ_2 == 0);
			ಗಾಗಿ(addr_t i = 0; i < ಪದಗಳು_2; i++) {
				RTLIL::Const ಹಿಂದಿನ = (*ಇದು)[addr + i];
				ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j = 0; j < ದತ್ತಾಂಶ_ಅಗಲ_2; j++)
					ಆದರೆ(init.en[j] != State::S1)
						ದತ್ತಾಂಶ.ಗಣ(ದತ್ತಾಂಶ_ಅಗಲ_2 * i + j, ಹಿಂದಿನ[j]);
			}
			ಸೇರಿಸು_concatenated(init.addr.as_int(), ದತ್ತಾಂಶ);
		}
	}
}

MemContents::iterator & MemContents::iterator::ಕ್ರಿಯಾಕಾರಕ++() {
	ಸ್ವಯಂಚಾಲಿತ it = ಸ್ಮರಣೆ_2->ಮೌಲ್ಯಗಳು_2.ಊರ್ಧ್ವ_bound(_addr);
	ಆದರೆ(it == ಸ್ಮರಣೆ_2->ಮೌಲ್ಯಗಳು_2.ಕೊನೆ()) {
		ಸ್ಮರಣೆ_2 = ಶೂನ್ಯಸೂಚಕ;
		_addr = ~(addr_t) 0;
	} ಇಲ್ಲದಿದ್ದರೆ
		_addr = it->ಮೊದಲ;
	ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ MemContents::check() {
	log_assert(addr_ಅಗಲ_2 > 0 && addr_ಅಗಲ_2 < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(addr_t) * 8);
	log_assert(ದತ್ತಾಂಶ_ಅಗಲ_2 > 0);
	log_assert(default_ಮೌಲ್ಯ.ಗಾತ್ರ_2() == ದತ್ತಾಂಶ_ಅಗಲ_2);
	ಆದರೆ(ಮೌಲ್ಯಗಳು_2.ಖಾಲಿಗಳು()) ಹಿಂತಿರುಗಿ;
	ಸ್ವಯಂಚಾಲಿತ it = ಮೌಲ್ಯಗಳು_2.begin();
	ಗಾಗಿ(;;) {
		log_assert(!it->ಎರಡನೇ.ಖಾಲಿಗಳು());
		log_assert(it->ಎರಡನೇ.ಗಾತ್ರ_2() % ದತ್ತಾಂಶ_ಅಗಲ_2 == 0);
		ಸ್ವಯಂಚಾಲಿತ end1 = range_ಕೊನೆ(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_ಅಗಲ_2));
		log_assert(end1 <= (addr_t)(1<<addr_ಅಗಲ_2));
		ಆದರೆ(++it == ಮೌಲ್ಯಗಳು_2.ಕೊನೆ())
			ನಿಲ್ಲಿಸು;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

ತಾರ್ಕಿಕ MemContents::range_ಒಳಗೊಂಡಿದೆ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ಸ್ಥಿರಾಂಕ {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	ಹಿಂತಿರುಗಿ it != ಮೌಲ್ಯಗಳು_2.ಕೊನೆ() && addr - _range_begin(it) < range_ಗಾತ್ರ(it);
}


ತಾರ್ಕಿಕ MemContents::range_ಒಳಗೊಂಡಿದೆ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ {
	// note that we assume begin_addr <= end_addr
	ಹಿಂತಿರುಗಿ it != ಮೌಲ್ಯಗಳು_2.ಕೊನೆ() && _range_begin(it) <= begin_addr && ಕೊನೆ_addr - _range_begin(it) <= range_ಗಾತ್ರ(it);
}

ತಾರ್ಕಿಕ MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ(it == ಮೌಲ್ಯಗಳು_2.ಕೊನೆ() || begin_addr >= ಕೊನೆ_addr)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಸ್ವಯಂಚಾಲಿತ top1 = range_ಕೊನೆ(it) - 1;
	ಸ್ವಯಂಚಾಲಿತ top2 = ಕೊನೆ_addr - 1;
	ಹಿಂತಿರುಗಿ !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ಸ್ಥಿರಾಂಕ {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_ಅಗಲ_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	ಸ್ವಯಂಚಾಲಿತ it = const_cast<std::map<addr_t, RTLIL::Const> &>(ಮೌಲ್ಯಗಳು_2).ಊರ್ಧ್ವ_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	ಆದರೆ(it == ಮೌಲ್ಯಗಳು_2.begin())
		ಹಿಂತಿರುಗಿ it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	ಸ್ವಯಂಚಾಲಿತ it_prev = std::ಮುಂದಿನ(it, -1);
	ಆದರೆ(range_ಒಳಗೊಂಡಿದೆ(it_prev, addr))
		ಹಿಂತಿರುಗಿ it_prev;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ it;
}

RTLIL::Const MemContents::ಕ್ರಿಯಾಕಾರಕ[](addr_t addr) ಸ್ಥಿರಾಂಕ {
	ಸ್ವಯಂಚಾಲಿತ it = _range_at(addr);
	ಆದರೆ(range_ಒಳಗೊಂಡಿದೆ(it, addr))
		ಹಿಂತಿರುಗಿ it->ಎರಡನೇ.ಹೊರತೆಗೆ(range_ಸ್ಥಾನಾಂತರ(it, addr), ದತ್ತಾಂಶ_ಅಗಲ_2);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ;
}

MemContents::addr_t MemContents::ಎಣಿಕೆ_range(addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ {
	addr_t ಎಣಿಕೆ_2 = 0;
	ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ it = _range_at(begin_addr); _range_overlaps(it, begin_addr, ಕೊನೆ_addr); it++) {
		ಸ್ವಯಂಚಾಲಿತ ಮೊದಲ = std::max(_range_begin(it), begin_addr);
		ಸ್ವಯಂಚಾಲಿತ ಕೊನೆಯ = std::min(range_ಕೊನೆ(it), ಕೊನೆ_addr);
		ಎಣಿಕೆ_2 += ಕೊನೆಯ - ಮೊದಲ;
	}
	ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

ಶೂನ್ಯ MemContents::clear_range(addr_t begin_addr, addr_t ಕೊನೆ_addr) {
	ಆದರೆ(begin_addr >= ಕೊನೆ_addr) ಹಿಂತಿರುಗಿ;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	ಸ್ವಯಂಚಾಲಿತ begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ_it = ಮೌಲ್ಯಗಳು_2.ಊರ್ಧ್ವ_bound(ಕೊನೆ_addr - 1);
	ಆದರೆ(begin_it == ಕೊನೆ_it)
		ಹಿಂತಿರುಗಿ; // nothing to do
	// the last iterator affected is one before the first one not affected
	ಸ್ವಯಂಚಾಲಿತ ಕೊನೆಯ_it = std::ಮುಂದಿನ(ಕೊನೆ_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	ಆದರೆ(range_ಒಳಗೊಂಡಿದೆ(ಕೊನೆಯ_it, ಕೊನೆ_addr - 1)) {
		ಸ್ವಯಂಚಾಲಿತ new_begin = ಕೊನೆ_addr;
		ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ = range_ಕೊನೆ(ಕೊನೆಯ_it);
		// if there is data past the end address, preserve it by creating a new range
		ಆದರೆ(new_begin != ಕೊನೆ)
			ಕೊನೆ_it = ಮೌಲ್ಯಗಳು_2.emplace_hint(ಕೊನೆಯ_it, new_begin, ಕೊನೆಯ_it->ಎರಡನೇ.ಹೊರತೆಗೆ(range_ಸ್ಥಾನಾಂತರ(ಕೊನೆಯ_it, new_begin), (range_ಕೊನೆ(ಕೊನೆಯ_it) - new_begin) * ದತ್ತಾಂಶ_ಅಗಲ_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	ಆದರೆ(range_ಒಳಗೊಂಡಿದೆ(begin_it, begin_addr)) {
		ಸ್ವಯಂಚಾಲಿತ new_ಕೊನೆ = begin_addr;
		// if there is data before the start address, truncate but don't delete
		ಆದರೆ(new_ಕೊನೆ != begin_it->ಮೊದಲ) {
			begin_it->ಎರಡನೇ.extu(range_ಸ್ಥಾನಾಂತರ(begin_it, new_ಕೊನೆ));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	ಮೌಲ್ಯಗಳು_2.erase(begin_it, ಕೊನೆ_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t ಕೊನೆ_addr) {
	ಆದರೆ(begin_addr >= ಕೊನೆ_addr)
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.ಕೊನೆ(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	ಸ್ವಯಂಚಾಲಿತ ಅಧೋ_it = begin_addr == 0 ? ಮೌಲ್ಯಗಳು_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	ಆದರೆ (range_ಒಳಗೊಂಡಿದೆ(ಅಧೋ_it, begin_addr, ಕೊನೆ_addr))
		ಹಿಂತಿರುಗಿ ಅಧೋ_it;
	// find the first range containing any addr >= end_addr
	ಸ್ವಯಂಚಾಲಿತ ಊರ್ಧ್ವ_it = _range_at(ಕೊನೆ_addr);
	// check if either of the two ranges we just found touch our range
	ತಾರ್ಕಿಕ ಅಧೋ_touch = begin_addr > 0 && range_ಒಳಗೊಂಡಿದೆ(ಅಧೋ_it, begin_addr - 1);
	ತಾರ್ಕಿಕ ಊರ್ಧ್ವ_touch = range_ಒಳಗೊಂಡಿದೆ(ಊರ್ಧ್ವ_it, ಕೊನೆ_addr);
	ಆದರೆ (ಅಧೋ_touch && ಊರ್ಧ್ವ_touch) {
		log_assert (ಅಧೋ_it != ಊರ್ಧ್ವ_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		ಸ್ವಯಂಚಾಲಿತ ಊರ್ಧ್ವ_ಕೊನೆ = range_ಕೊನೆ(ಊರ್ಧ್ವ_it);
		// make range bigger (maybe reserve here instead of resize?)
		ಅಧೋ_it->ಎರಡನೇ.resize(range_ಸ್ಥಾನಾಂತರ(ಅಧೋ_it, ಊರ್ಧ್ವ_ಕೊನೆ), State::Sx);
		// copy only the data beyond our range
		std::copy(range_ದತ್ತಾಂಶ(ಊರ್ಧ್ವ_it, ಕೊನೆ_addr), range_ದತ್ತಾಂಶ(ಊರ್ಧ್ವ_it, ಊರ್ಧ್ವ_ಕೊನೆ), range_ದತ್ತಾಂಶ(ಅಧೋ_it, ಕೊನೆ_addr));
		// keep lower_it, but delete upper_it
		ಮೌಲ್ಯಗಳು_2.erase(std::ಮುಂದಿನ(ಅಧೋ_it), std::ಮುಂದಿನ(ಊರ್ಧ್ವ_it));
		ಹಿಂತಿರುಗಿ ಅಧೋ_it;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಧೋ_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		ಅಧೋ_it->ಎರಡನೇ.resize(range_ಸ್ಥಾನಾಂತರ(ಅಧೋ_it, ಕೊನೆ_addr), State::Sx);
		// keep lower_it and upper_it
		ಮೌಲ್ಯಗಳು_2.erase(std::ಮುಂದಿನ(ಅಧೋ_it), ಊರ್ಧ್ವ_it);
		ಹಿಂತಿರುಗಿ ಅಧೋ_it;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಊರ್ಧ್ವ_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const ದತ್ತಾಂಶ(State::Sx, (_range_begin(ಊರ್ಧ್ವ_it) - begin_addr) * ದತ್ತಾಂಶ_ಅಗಲ_2);
		ದತ್ತಾಂಶ.append(std::move(ಊರ್ಧ್ವ_it->ಎರಡನೇ));
		// delete lower_it and upper_it, then reinsert
		ಮೌಲ್ಯಗಳು_2.erase(ಅಧೋ_it, std::ಮುಂದಿನ(ಊರ್ಧ್ವ_it));
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.emplace(begin_addr, std::move(ದತ್ತಾಂಶ)).ಮೊದಲ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		ಮೌಲ್ಯಗಳು_2.erase(ಅಧೋ_it, ಊರ್ಧ್ವ_it);
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.emplace(begin_addr, RTLIL::Const(State::Sx, (ಕೊನೆ_addr - begin_addr) * ದತ್ತಾಂಶ_ಅಗಲ_2)).ಮೊದಲ;
	}
}

ಶೂನ್ಯ MemContents::ಸೇರಿಸು_concatenated(addr_t addr, RTLIL::Const ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯಗಳು) {
	addr_t ಪದಗಳು_2 = (ಮೌಲ್ಯಗಳು.ಗಾತ್ರ_2() + ದತ್ತಾಂಶ_ಅಗಲ_2 - 1) / ದತ್ತಾಂಶ_ಅಗಲ_2;
	log_assert(addr < (addr_t)(1<<addr_ಅಗಲ_2));
	log_assert(ಪದಗಳು_2 <= (addr_t)(1<<addr_ಅಗಲ_2) - addr);
	ಸ್ವಯಂಚಾಲಿತ it = _reserve_range(addr, addr + ಪದಗಳು_2);
	ಸ್ವಯಂಚಾಲಿತ ಗಮ್ಯ_begin = range_ದತ್ತಾಂಶ(it, addr);
	std::copy(ಮೌಲ್ಯಗಳು.begin(), ಮೌಲ್ಯಗಳು.ಕೊನೆ(), ಗಮ್ಯ_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(ಗಮ್ಯ_begin + ಮೌಲ್ಯಗಳು.ಗಾತ್ರ_2(), ಗಮ್ಯ_begin + ಪದಗಳು_2 * ದತ್ತಾಂಶ_ಅಗಲ_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ಸ್ಥಿರಾಂಕ &ಶಬ್ದ) {
	ಸ್ವಯಂಚಾಲಿತ from_ಕೊನೆ = ಶಬ್ದ.ಗಾತ್ರ_2() <= ದತ್ತಾಂಶ_ಅಗಲ_2 ? ಶಬ್ದ.ಕೊನೆ() : ಶಬ್ದ.begin() + ದತ್ತಾಂಶ_ಅಗಲ_2;
	ಸ್ವಯಂಚಾಲಿತ ಗಮ್ಯ_ಕೊನೆ = std::copy(ಶಬ್ದ.begin(), from_ಕೊನೆ, it);
	ಸ್ವಯಂಚಾಲಿತ it_ಮುಂದಿನ = std::ಮುಂದಿನ(it, ದತ್ತಾಂಶ_ಅಗಲ_2);
	std::fill(ಗಮ್ಯ_ಕೊನೆ, it_ಮುಂದಿನ, State::S0);
	ಹಿಂತಿರುಗಿ it_ಮುಂದಿನ;
}
