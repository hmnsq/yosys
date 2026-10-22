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

#अन्तर्भाव "kernel/mem.h"
#अन्तर्भाव "kernel/ff.h"

USING_YOSYS_NAMESPACE

शून्य Mem::remove() {
	यदि (cell) {
		module->remove(cell);
		cell = शून्यसूचकः;
	}
	यदि (mem) {
		module->memories.erase(mem->name);
		विलोपय mem;
		mem = शून्यसूचकः;
	}
	प्रति (स्वचालित &पत्तनम् : rd_ports) {
		यदि (पत्तनम्.cell) {
			module->remove(पत्तनम्.cell);
			पत्तनम्.cell = शून्यसूचकः;
		}
	}
	प्रति (स्वचालित &पत्तनम् : wr_ports) {
		यदि (पत्तनम्.cell) {
			module->remove(पत्तनम्.cell);
			पत्तनम्.cell = शून्यसूचकः;
		}
	}
	प्रति (स्वचालित &init : inits) {
		यदि (init.cell) {
			module->remove(init.cell);
			init.cell = शून्यसूचकः;
		}
	}
}

शून्य Mem::emit() {
	check();
	std::सदिशम्<पूर्णाङ्क> rd_वामः;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++) {
		स्वचालित &पत्तनम् = rd_ports[i];
		यदि (पत्तनम्.removed) {
			यदि (पत्तनम्.cell) {
				module->remove(पत्तनम्.cell);
			}
		} अन्यथा {
			rd_वामः.प्रक्षिप_पृष्ठभागः(i);
		}
	}
	std::सदिशम्<पूर्णाङ्क> wr_वामः;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
		स्वचालित &पत्तनम् = wr_ports[i];
		यदि (पत्तनम्.removed) {
			यदि (पत्तनम्.cell) {
				module->remove(पत्तनम्.cell);
			}
		} अन्यथा {
			wr_वामः.प्रक्षिप_पृष्ठभागः(i);
		}
	}
	std::सदिशम्<पूर्णाङ्क> init_वामः;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(inits); i++) {
		स्वचालित &init = inits[i];
		यदि (init.removed) {
			यदि (init.cell) {
				module->remove(init.cell);
			}
		} अन्यथा {
			init_वामः.प्रक्षिप_पृष्ठभागः(i);
		}
	}
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_वामः); i++)
		यदि (i != rd_वामः[i])
			std::swap(rd_ports[i], rd_ports[rd_वामः[i]]);
	rd_ports.resize(Get_आकार(rd_वामः));
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_वामः); i++)
		यदि (i != wr_वामः[i])
			std::swap(wr_ports[i], wr_ports[wr_वामः[i]]);
	wr_ports.resize(Get_आकार(wr_वामः));
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(init_वामः); i++)
		यदि (i != init_वामः[i])
			std::swap(inits[i], inits[init_वामः[i]]);
	inits.resize(Get_आकार(init_वामः));

	प्रति (स्वचालित &पत्तनम् : rd_ports) {
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_वामः); i++) {
			पत्तनम्.transparency_आवरणम्[i] = पत्तनम्.transparency_आवरणम्[wr_वामः[i]];
			पत्तनम्.collision_x_आवरणम्[i] = पत्तनम्.collision_x_आवरणम्[wr_वामः[i]];
		}
		पत्तनम्.transparency_आवरणम्.resize(Get_आकार(wr_वामः));
		पत्तनम्.collision_x_आवरणम्.resize(Get_आकार(wr_वामः));
	}
	प्रति (स्वचालित &पत्तनम् : wr_ports) {
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_वामः); i++)
			पत्तनम्.प्राथमिकता_आवरणम्[i] = पत्तनम्.प्राथमिकता_आवरणम्[wr_वामः[i]];
		पत्तनम्.प्राथमिकता_आवरणम्.resize(Get_आकार(wr_वामः));
	}

	यदि (packed) {
		यदि (mem) {
			module->memories.erase(mem->name);
			विलोपय mem;
			mem = शून्यसूचकः;
		}
		यदि (!cell) {
			यदि (memid.रिक्तानि())
				memid = PooledName(module->design, module->twines().सङ्कलय(NEW_ID));
			cell = module->सङ्कलय_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->गुणाः = गुणाः;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::विस्तारः_2] = Const(विस्तारः);
		cell->parameters[ID::विस्थापनम्_2] = Const(start_विस्थापनम्);
		cell->parameters[ID::आकार_2] = Const(आकार);
		SigSpec rd_clk, rd_en, rd_addr, rd_दत्त;
		SigSpec wr_clk, wr_en, wr_addr, wr_दत्त;
		SigSpec rd_arst, rd_srst;
		पूर्णाङ्क abits = 0;
		प्रति (स्वचालित &पत्तनम् : rd_ports)
			abits = std::max(abits, Get_आकार(पत्तनम्.addr));
		प्रति (स्वचालित &पत्तनम् : wr_ports)
			abits = std::max(abits, Get_आकार(पत्तनम्.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::सदिशम्<पूर्णाङ्क> wr_पत्तनम्_xlat;
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++)
			प्रति (पूर्णाङ्क j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_पत्तनम्_xlat.प्रक्षिप_पृष्ठभागः(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_आवरणम्_builder;
		Const::Builder rd_collision_x_आवरणम्_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_मानम्_builder;
		Const::Builder rd_srst_मानम्_builder;
		Const::Builder rd_init_मानम्_builder;
		प्रति (स्वचालित &पत्तनम् : rd_ports) {
			प्रति (स्वचालित attr: पत्तनम्.गुणाः)
				यदि (!cell->has_attribute(attr.first))
					cell->गुणाः.निवेशय(attr);
			यदि (पत्तनम्.cell) {
				module->remove(पत्तनम्.cell);
				पत्तनम्.cell = शून्यसूचकः;
			}
			प्रति (पूर्णाङ्क sub = 0; sub < (1 << पत्तनम्.wide_log2); sub++)
			{
				rd_wide_continuation_builder.प्रक्षिप_पृष्ठभागः(State(sub != 0));
				rd_clk_enable_builder.प्रक्षिप_पृष्ठभागः(State(पत्तनम्.clk_enable));
				rd_clk_polarity_builder.प्रक्षिप_पृष्ठभागः(State(पत्तनम्.clk_polarity));
				rd_ce_over_srst_builder.प्रक्षिप_पृष्ठभागः(State(पत्तनम्.ce_over_srst));
				rd_clk.append(पत्तनम्.clk);
				rd_arst.append(पत्तनम्.arst);
				rd_srst.append(पत्तनम्.srst);
				rd_en.append(पत्तनम्.en);
				SigSpec addr = पत्तनम्.sub_addr(sub);
				addr.extend_u0(abits, असत्य);
				rd_addr.append(addr);
				log_assert(Get_आकार(addr) == abits);
				प्रति (स्वचालित idx : wr_पत्तनम्_xlat) {
					rd_transparency_आवरणम्_builder.प्रक्षिप_पृष्ठभागः(State(तार्किक(पत्तनम्.transparency_आवरणम्[idx])));
					rd_collision_x_आवरणम्_builder.प्रक्षिप_पृष्ठभागः(State(तार्किक(पत्तनम्.collision_x_आवरणम्[idx])));
				}
			}
			rd_दत्त.append(पत्तनम्.दत्त);
			प्रति (स्वचालित bit : पत्तनम्.arst_मानम्)
				rd_arst_मानम्_builder.प्रक्षिप_पृष्ठभागः(bit);
			प्रति (स्वचालित bit : पत्तनम्.srst_मानम्)
				rd_srst_मानम्_builder.प्रक्षिप_पृष्ठभागः(bit);
			प्रति (स्वचालित bit : पत्तनम्.init_मानम्)
				rd_init_मानम्_builder.प्रक्षिप_पृष्ठभागः(bit);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_आवरणम् = rd_transparency_आवरणम्_builder.build();
		Const rd_collision_x_आवरणम् = rd_collision_x_आवरणम्_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_मानम् = rd_arst_मानम्_builder.build();
		Const rd_srst_मानम् = rd_srst_मानम्_builder.build();
		Const rd_init_मानम् = rd_init_मानम्_builder.build();
		यदि (rd_ports.रिक्तानि()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_मानम् = State::S0;
			rd_srst_मानम् = State::S0;
			rd_init_मानम् = State::S0;
		}
		यदि (rd_ports.रिक्तानि() || wr_ports.रिक्तानि()) {
			rd_transparency_आवरणम् = State::S0;
			rd_collision_x_आवरणम् = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_आकार(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_आवरणम्] = rd_transparency_आवरणम्;
		cell->parameters[ID::RD_COLLISION_X_आवरणम्] = rd_collision_x_आवरणम्;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_मानम्] = rd_arst_मानम्;
		cell->parameters[ID::RD_SRST_मानम्] = rd_srst_मानम्;
		cell->parameters[ID::RD_INIT_मानम्] = rd_init_मानम्;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->समुच्चयः_पत्तनम्(ID::RD_CLK, rd_clk);
		cell->समुच्चयः_पत्तनम्(ID::RD_EN, rd_en);
		cell->समुच्चयः_पत्तनम्(ID::RD_ARST, rd_arst);
		cell->समुच्चयः_पत्तनम्(ID::RD_SRST, rd_srst);
		cell->समुच्चयः_पत्तनम्(ID::RD_ADDR, rd_addr);
		cell->समुच्चयः_पत्तनम्(ID::RD_दत्त, rd_दत्त);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_प्राथमिकता_आवरणम्_builder;
		प्रति (स्वचालित &पत्तनम् : wr_ports) {
			प्रति (स्वचालित attr: पत्तनम्.गुणाः)
				यदि (!cell->has_attribute(attr.first))
					cell->गुणाः.निवेशय(attr);
			यदि (पत्तनम्.cell) {
				module->remove(पत्तनम्.cell);
				पत्तनम्.cell = शून्यसूचकः;
			}
			प्रति (पूर्णाङ्क sub = 0; sub < (1 << पत्तनम्.wide_log2); sub++)
			{
				wr_wide_continuation_builder.प्रक्षिप_पृष्ठभागः(State(sub != 0));
				wr_clk_enable_builder.प्रक्षिप_पृष्ठभागः(State(पत्तनम्.clk_enable));
				wr_clk_polarity_builder.प्रक्षिप_पृष्ठभागः(State(पत्तनम्.clk_polarity));
				wr_clk.append(पत्तनम्.clk);
				प्रति (स्वचालित idx : wr_पत्तनम्_xlat)
					wr_प्राथमिकता_आवरणम्_builder.प्रक्षिप_पृष्ठभागः(State(तार्किक(पत्तनम्.प्राथमिकता_आवरणम्[idx])));
				SigSpec addr = पत्तनम्.sub_addr(sub);
				addr.extend_u0(abits, असत्य);
				wr_addr.append(addr);
				log_assert(Get_आकार(addr) == abits);
			}
			wr_en.append(पत्तनम्.en);
			wr_दत्त.append(पत्तनम्.दत्त);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_प्राथमिकता_आवरणम् = wr_प्राथमिकता_आवरणम्_builder.build();
		यदि (wr_ports.रिक्तानि()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_प्राथमिकता_आवरणम् = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_आकार(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_प्राथमिकता_आवरणम्] = wr_प्राथमिकता_आवरणम्;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->समुच्चयः_पत्तनम्(ID::WR_CLK, wr_clk);
		cell->समुच्चयः_पत्तनम्(ID::WR_EN, wr_en);
		cell->समुच्चयः_पत्तनम्(ID::WR_ADDR, wr_addr);
		cell->समुच्चयः_पत्तनम्(ID::WR_दत्त, wr_दत्त);
		प्रति (स्वचालित &init : inits) {
			प्रति (स्वचालित attr: init.गुणाः)
				यदि (!cell->has_attribute(attr.first))
					cell->गुणाः.निवेशय(attr);
			यदि (init.cell) {
				module->remove(init.cell);
				init.cell = शून्यसूचकः;
			}
		}
		cell->parameters[ID::INIT] = get_init_दत्त();
	} अन्यथा {
		यदि (cell) {
			module->remove(cell);
			cell = शून्यसूचकः;
		}
		यदि (!mem) {
			यदि (memid.रिक्तानि())
				memid = PooledName(module->design, module->twines().सङ्कलय(NEW_ID));
			mem = module->सङ्कलय_स्मृतिः(memid);
		}
		mem->विस्तारः = विस्तारः;
		mem->start_विस्थापनम् = start_विस्थापनम्;
		mem->आकार = आकार;
		mem->गुणाः = गुणाः;
		प्रति (स्वचालित &पत्तनम् : rd_ports) {
			यदि (!पत्तनम्.cell)
				पत्तनम्.cell = module->सङ्कलय_Cell(NEW_ID, ID($memrd_v2));
			पत्तनम्.cell->type = ID($memrd_v2);
			पत्तनम्.cell->गुणाः = पत्तनम्.गुणाः;
			पत्तनम्.cell->parameters[ID::MEMID] = memid.str();
			पत्तनम्.cell->parameters[ID::ABITS] = Get_आकार(पत्तनम्.addr);
			पत्तनम्.cell->parameters[ID::विस्तारः_2] = विस्तारः << पत्तनम्.wide_log2;
			पत्तनम्.cell->parameters[ID::CLK_ENABLE] = पत्तनम्.clk_enable;
			पत्तनम्.cell->parameters[ID::CLK_POLARITY] = पत्तनम्.clk_polarity;
			पत्तनम्.cell->parameters[ID::CE_OVER_SRST] = पत्तनम्.ce_over_srst;
			पत्तनम्.cell->parameters[ID::ARST_मानम्] = पत्तनम्.arst_मानम्;
			पत्तनम्.cell->parameters[ID::SRST_मानम्] = पत्तनम्.srst_मानम्;
			पत्तनम्.cell->parameters[ID::INIT_मानम्] = पत्तनम्.init_मानम्;
			पत्तनम्.cell->parameters[ID::TRANSPARENCY_आवरणम्] = पत्तनम्.transparency_आवरणम्;
			पत्तनम्.cell->parameters[ID::COLLISION_X_आवरणम्] = पत्तनम्.collision_x_आवरणम्;
			पत्तनम्.cell->parameters.erase(ID::TRANSPARENT);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::CLK, पत्तनम्.clk);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::EN, पत्तनम्.en);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::ARST, पत्तनम्.arst);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::SRST, पत्तनम्.srst);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::ADDR, पत्तनम्.addr);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::दत्त_2, पत्तनम्.दत्त);
		}
		पूर्णाङ्क idx = 0;
		प्रति (स्वचालित &पत्तनम् : wr_ports) {
			यदि (!पत्तनम्.cell)
				पत्तनम्.cell = module->सङ्कलय_Cell(NEW_ID, ID($memwr_v2));
			पत्तनम्.cell->type = ID($memwr_v2);
			पत्तनम्.cell->गुणाः = पत्तनम्.गुणाः;
			यदि (पत्तनम्.cell->parameters.गणनम्(ID::प्राथमिकता))
				पत्तनम्.cell->parameters.erase(ID::प्राथमिकता);
			पत्तनम्.cell->parameters[ID::MEMID] = memid.str();
			पत्तनम्.cell->parameters[ID::ABITS] = Get_आकार(पत्तनम्.addr);
			पत्तनम्.cell->parameters[ID::विस्तारः_2] = विस्तारः << पत्तनम्.wide_log2;
			पत्तनम्.cell->parameters[ID::CLK_ENABLE] = पत्तनम्.clk_enable;
			पत्तनम्.cell->parameters[ID::CLK_POLARITY] = पत्तनम्.clk_polarity;
			पत्तनम्.cell->parameters[ID::PORTID] = idx++;
			पत्तनम्.cell->parameters[ID::प्राथमिकता_आवरणम्_2] = पत्तनम्.प्राथमिकता_आवरणम्;
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::CLK, पत्तनम्.clk);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::EN, पत्तनम्.en);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::ADDR, पत्तनम्.addr);
			पत्तनम्.cell->समुच्चयः_पत्तनम्(ID::दत्त_2, पत्तनम्.दत्त);
		}
		idx = 0;
		प्रति (स्वचालित &init : inits) {
			तार्किक v2 = !init.en.is_fully_ones();
			यदि (!init.cell)
				init.cell = module->सङ्कलय_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			अन्यथा {
				यदि (!v2)
					init.cell->unset_पत्तनम्(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->गुणाः = init.गुणाः;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_आकार(init.addr);
			init.cell->parameters[ID::विस्तारः_2] = विस्तारः;
			init.cell->parameters[ID::शब्दाः] = Get_आकार(init.दत्त) / विस्तारः;
			init.cell->parameters[ID::प्राथमिकता] = idx++;
			init.cell->समुच्चयः_पत्तनम्(ID::ADDR, init.addr);
			init.cell->समुच्चयः_पत्तनम्(ID::दत्त_2, init.दत्त);
			यदि (v2)
				init.cell->समुच्चयः_पत्तनम्(ID::EN, init.en);
		}
	}
}

शून्य Mem::clear_inits() {
	प्रति (स्वचालित &init : inits)
		init.removed = सत्य;
}

शून्य Mem::coalesce_inits() {
	// start address -> end address
	std::map<पूर्णाङ्क, पूर्णाङ्क> chunks;
	// Figure out chunk boundaries.
	प्रति (स्वचालित &init : inits) {
		यदि (init.removed)
			अनुवर्त;
		तार्किक valid = असत्य;
		प्रति (स्वचालित bit : init.en)
			यदि (bit == State::S1)
				valid = सत्य;
		यदि (!valid) {
			init.removed = सत्य;
			अनुवर्त;
		}
		पूर्णाङ्क addr = init.addr.as_int();
		पूर्णाङ्क addr_e = addr + Get_आकार(init.दत्त) / विस्तारः;
		स्वचालित it_e = chunks.ऊर्ध्वस्थम्_bound(addr_e);
		स्वचालित it = it_e;
		यावत् (it != chunks.begin()) {
			--it;
			यदि (it->second < addr) {
				++it;
				भङ्ग;
			}
		}
		यदि (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} अन्यथा {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			यदि (it->first < addr)
				addr = it->first;
			स्वचालित it_चरमः = it_e;
			it_चरमः--;
			यदि (it_चरमः->second > addr_e)
				addr_e = it_चरमः->second;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<पूर्णाङ्क, std::सदिशम्<पूर्णाङ्क>> inits_by_chunk;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(inits); i++) {
		स्वचालित &init = inits[i];
		यदि (init.removed)
			अनुवर्त;
		स्वचालित it = chunks.ऊर्ध्वस्थम्_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->first].प्रक्षिप_पृष्ठभागः(i);
		पूर्णाङ्क addr = init.addr.as_int();
		पूर्णाङ्क addr_e = addr + Get_आकार(init.दत्त) / विस्तारः;
		log_assert(addr >= it->first && addr_e <= it->second);
	}
	// Process each chunk.
	प्रति (स्वचालित &it : inits_by_chunk) {
		पूर्णाङ्क caddr = it.first;
		पूर्णाङ्क caddr_e = chunks[caddr];
		स्वचालित &chunk_inits = it.second;
		यदि (Get_आकार(chunk_inits) == 1) {
			स्वचालित &init = inits[chunk_inits[0]];
			यदि (!init.en.is_fully_ones()) {
				प्रति (पूर्णाङ्क i = 0; i < Get_आकार(init.दत्त); i++)
					यदि (init.en[i % विस्तारः] != State::S1)
						init.दत्त.समुच्चयः(i, State::Sx);
				init.en = Const(State::S1, विस्तारः);
			}
			अनुवर्त;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * विस्तारः);
		प्रति (पूर्णाङ्क idx : chunk_inits) {
			स्वचालित &init = inits[idx];
			पूर्णाङ्क विस्थापनम् = (init.addr.as_int() - caddr) * विस्तारः;
			log_assert(विस्थापनम् >= 0);
			log_assert(विस्थापनम् + Get_आकार(init.दत्त) <= Get_आकार(cdata));
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(init.दत्त); i++)
				यदि (init.en[i % विस्तारः] == State::S1)
					cdata.समुच्चयः(i+विस्थापनम्, init.दत्त[i]);
			init.removed = सत्य;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.दत्त = cdata;
		new_init.en = Const(State::S1, विस्तारः);
		inits.प्रक्षिप_पृष्ठभागः(new_init);
	}
}

Const Mem::get_init_दत्त() ध्रुव {
	Const init_दत्त(State::Sx, विस्तारः * आकार);
	प्रति (स्वचालित &init : inits) {
		यदि (init.removed)
			अनुवर्त;
		पूर्णाङ्क विस्थापनम् = (init.addr.as_int() - start_विस्थापनम्) * विस्तारः;
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(init.दत्त); i++)
			यदि (0 <= i+विस्थापनम् && i+विस्थापनम् < Get_आकार(init_दत्त) && init.en[i % विस्तारः] == State::S1)
				init_दत्त.समुच्चयः(i+विस्थापनम्, init.दत्त[i]);
	}
	प्रत्यागम init_दत्त;
}

शून्य Mem::check() {
	पूर्णाङ्क max_wide_log2 = 0;
	प्रति (स्वचालित &पत्तनम् : rd_ports) {
		यदि (पत्तनम्.removed)
			अनुवर्त;
		log_assert(Get_आकार(पत्तनम्.clk) == 1);
		log_assert(Get_आकार(पत्तनम्.en) == 1);
		log_assert(Get_आकार(पत्तनम्.arst) == 1);
		log_assert(Get_आकार(पत्तनम्.srst) == 1);
		log_assert(Get_आकार(पत्तनम्.addr) >= पत्तनम्.wide_log2);
		log_assert(Get_आकार(पत्तनम्.दत्त) == (विस्तारः << पत्तनम्.wide_log2));
		log_assert(Get_आकार(पत्तनम्.init_मानम्) == (विस्तारः << पत्तनम्.wide_log2));
		log_assert(Get_आकार(पत्तनम्.arst_मानम्) == (विस्तारः << पत्तनम्.wide_log2));
		log_assert(Get_आकार(पत्तनम्.srst_मानम्) == (विस्तारः << पत्तनम्.wide_log2));
		यदि (!पत्तनम्.clk_enable) {
			log_assert(पत्तनम्.en == State::S1);
			log_assert(पत्तनम्.arst == State::S0);
			log_assert(पत्तनम्.srst == State::S0);
		}
		प्रति (पूर्णाङ्क j = 0; j < पत्तनम्.wide_log2; j++) {
			log_assert(पत्तनम्.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, पत्तनम्.wide_log2);
		log_assert(Get_आकार(पत्तनम्.transparency_आवरणम्) == Get_आकार(wr_ports));
		log_assert(Get_आकार(पत्तनम्.collision_x_आवरणम्) == Get_आकार(wr_ports));
		प्रति (पूर्णाङ्क j = 0; j < Get_आकार(wr_ports); j++) {
			स्वचालित &wport = wr_ports[j];
			यदि ((पत्तनम्.transparency_आवरणम्[j] || पत्तनम्.collision_x_आवरणम्[j]) && !wport.removed) {
				log_assert(पत्तनम्.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(पत्तनम्.clk == wport.clk);
				log_assert(पत्तनम्.clk_polarity == wport.clk_polarity);
			}
			log_assert(!पत्तनम्.transparency_आवरणम्[j] || !पत्तनम्.collision_x_आवरणम्[j]);
		}
	}
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
		स्वचालित &पत्तनम् = wr_ports[i];
		यदि (पत्तनम्.removed)
			अनुवर्त;
		log_assert(Get_आकार(पत्तनम्.clk) == 1);
		log_assert(Get_आकार(पत्तनम्.en) == (विस्तारः << पत्तनम्.wide_log2));
		log_assert(Get_आकार(पत्तनम्.दत्त) == (विस्तारः << पत्तनम्.wide_log2));
		log_assert(Get_आकार(पत्तनम्.addr) >= पत्तनम्.wide_log2);
		प्रति (पूर्णाङ्क j = 0; j < पत्तनम्.wide_log2; j++) {
			log_assert(पत्तनम्.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, पत्तनम्.wide_log2);
		log_assert(Get_आकार(पत्तनम्.प्राथमिकता_आवरणम्) == Get_आकार(wr_ports));
		प्रति (पूर्णाङ्क j = 0; j < Get_आकार(wr_ports); j++) {
			स्वचालित &wport = wr_ports[j];
			यदि (पत्तनम्.प्राथमिकता_आवरणम्[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(पत्तनम्.clk_enable == wport.clk_enable);
				यदि (पत्तनम्.clk_enable) {
					log_assert(पत्तनम्.clk == wport.clk);
					log_assert(पत्तनम्.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	पूर्णाङ्क आवरणम् = (1 << max_wide_log2) - 1;
	log_assert(!(start_विस्थापनम् & आवरणम्));
	log_assert(!(आकार & आवरणम्));
	log_assert(विस्तारः != 0);
}

नामाकाश {

	संरचना Mem_सूचकांक {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_सूचकांक (Module *module) {
			प्रति (स्वचालित cell: module->cells()) {
				यदि (cell->type.अन्तर्(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].निवेशय(cell);
				अन्यथा यदि (cell->type.अन्तर्(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].निवेशय(cell);
				अन्यथा यदि (cell->type.अन्तर्(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].निवेशय(cell);
			}
		}
	};

	Mem mem_from_स्मृतिः(Module *module, RTLIL::स्मृतिः *mem, ध्रुव Mem_सूचकांक &सूचकांक) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->विस्तारः, mem->start_विस्थापनम्, mem->आकार);
		res.packed = असत्य;
		res.mem = mem;
		res.गुणाः = mem->गुणाः;
		std::सदिशम्<तार्किक> rd_transparent;
		std::सदिशम्<पूर्णाङ्क> wr_portid;
		यदि (सूचकांक.rd_ports.गणनम्(memid)) {
			प्रति (स्वचालित cell : सूचकांक.rd_ports.at(memid)) {
				MemRd mrd;
				तार्किक is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.गुणाः = cell->गुणाः;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_पत्तनम्(ID::CLK);
				mrd.en = cell->get_पत्तनम्(ID::EN);
				mrd.addr = cell->get_पत्तनम्(ID::ADDR);
				mrd.दत्त = cell->get_पत्तनम्(ID::दत्त_2);
				mrd.wide_log2 = ceil_log2(Get_आकार(mrd.दत्त) / mem->विस्तारः);
				तार्किक transparent = असत्य;
				यदि (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = असत्य;
					mrd.arst_मानम् = Const(State::Sx, mem->विस्तारः << mrd.wide_log2);
					mrd.srst_मानम् = Const(State::Sx, mem->विस्तारः << mrd.wide_log2);
					mrd.init_मानम् = Const(State::Sx, mem->विस्तारः << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					यदि (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = असत्य;
						यदि (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} अन्यथा {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_मानम् = cell->parameters.at(ID::ARST_मानम्);
					mrd.srst_मानम् = cell->parameters.at(ID::SRST_मानम्);
					mrd.init_मानम् = cell->parameters.at(ID::INIT_मानम्);
					mrd.arst = cell->get_पत्तनम्(ID::ARST);
					mrd.srst = cell->get_पत्तनम्(ID::SRST);
				}
				res.rd_ports.प्रक्षिप_पृष्ठभागः(mrd);
				rd_transparent.प्रक्षिप_पृष्ठभागः(transparent);
			}
		}
		यदि (सूचकांक.wr_ports.गणनम्(memid)) {
			std::सदिशम्<std::pair<पूर्णाङ्क, MemWr>> ports;
			प्रति (स्वचालित cell : सूचकांक.wr_ports.at(memid)) {
				MemWr mwr;
				तार्किक is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.गुणाः = cell->गुणाः;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_पत्तनम्(ID::CLK);
				mwr.en = cell->get_पत्तनम्(ID::EN);
				mwr.addr = cell->get_पत्तनम्(ID::ADDR);
				mwr.दत्त = cell->get_पत्तनम्(ID::दत्त_2);
				mwr.wide_log2 = ceil_log2(Get_आकार(mwr.दत्त) / mem->विस्तारः);
				ports.प्रक्षिप_पृष्ठभागः(std::make_pair(cell->parameters.at(is_compat ? ID::प्राथमिकता : ID::PORTID).as_int(), mwr));
			}
			std::क्रमय(ports.begin(), ports.अन्तः(), [](ध्रुव std::pair<पूर्णाङ्क, MemWr> &a, ध्रुव std::pair<पूर्णाङ्क, MemWr> &b) { प्रत्यागम a.first < b.first; });
			प्रति (स्वचालित &it : ports) {
				res.wr_ports.प्रक्षिप_पृष्ठभागः(it.second);
				wr_portid.प्रक्षिप_पृष्ठभागः(it.first);
			}
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(res.wr_ports); i++) {
				स्वचालित &पत्तनम् = res.wr_ports[i];
				तार्किक is_compat = पत्तनम्.cell->type == ID($memwr);
				यदि (is_compat) {
					पत्तनम्.प्राथमिकता_आवरणम्.resize(Get_आकार(res.wr_ports));
					प्रति (पूर्णाङ्क j = 0; j < i; j++) {
						स्वचालित &oport = res.wr_ports[j];
						यदि (पत्तनम्.clk_enable != oport.clk_enable)
							अनुवर्त;
						यदि (पत्तनम्.clk_enable && पत्तनम्.clk != oport.clk)
							अनुवर्त;
						यदि (पत्तनम्.clk_enable && पत्तनम्.clk_polarity != oport.clk_polarity)
							अनुवर्त;
						पत्तनम्.प्राथमिकता_आवरणम्[j] = सत्य;
					}
				} अन्यथा {
					Const orig_prio_आवरणम् = पत्तनम्.cell->parameters.at(ID::प्राथमिकता_आवरणम्_2);
					प्रति (पूर्णाङ्क orig_portid : wr_portid) {
						तार्किक has_prio = orig_portid < Get_आकार(orig_prio_आवरणम्) && orig_prio_आवरणम्[orig_portid] == State::S1;
						पत्तनम्.प्राथमिकता_आवरणम्.प्रक्षिप_पृष्ठभागः(has_prio);
					}
				}
			}
		}
		यदि (सूचकांक.inits.गणनम्(memid)) {
			std::सदिशम्<std::pair<पूर्णाङ्क, MemInit>> inits;
			प्रति (स्वचालित cell : सूचकांक.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.गुणाः = cell->गुणाः;
				स्वचालित addr = cell->get_पत्तनम्(ID::ADDR);
				स्वचालित दत्त = cell->get_पत्तनम्(ID::दत्त_2);
				यदि (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				यदि (!दत्त.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(दत्त), cell);
				init.addr = addr.as_const();
				init.दत्त = दत्त.as_const();
				यदि (cell->type == ID($meminit_v2)) {
					स्वचालित en = cell->get_पत्तनम्(ID::EN);
					यदि (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} अन्यथा {
					init.en = RTLIL::Const(State::S1, mem->विस्तारः);
				}
				inits.प्रक्षिप_पृष्ठभागः(std::make_pair(cell->parameters.at(ID::प्राथमिकता).as_int(), init));
			}
			std::क्रमय(inits.begin(), inits.अन्तः(), [](ध्रुव std::pair<पूर्णाङ्क, MemInit> &a, ध्रुव std::pair<पूर्णाङ्क, MemInit> &b) { प्रत्यागम a.first < b.first; });
			प्रति (स्वचालित &it : inits)
				res.inits.प्रक्षिप_पृष्ठभागः(it.second);
		}
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(res.rd_ports); i++) {
			स्वचालित &पत्तनम् = res.rd_ports[i];
			तार्किक is_compat = पत्तनम्.cell->type == ID($memrd);
			यदि (is_compat) {
				पत्तनम्.transparency_आवरणम्.resize(Get_आकार(res.wr_ports));
				पत्तनम्.collision_x_आवरणम्.resize(Get_आकार(res.wr_ports));
				यदि (!rd_transparent[i])
					अनुवर्त;
				यदि (!पत्तनम्.clk_enable)
					अनुवर्त;
				प्रति (पूर्णाङ्क j = 0; j < Get_आकार(res.wr_ports); j++) {
					स्वचालित &wport = res.wr_ports[j];
					यदि (!wport.clk_enable)
						अनुवर्त;
					यदि (पत्तनम्.clk != wport.clk)
						अनुवर्त;
					यदि (पत्तनम्.clk_polarity != wport.clk_polarity)
						अनुवर्त;
					पत्तनम्.transparency_आवरणम्[j] = सत्य;
				}
			} अन्यथा {
				Const orig_trans_आवरणम् = पत्तनम्.cell->parameters.at(ID::TRANSPARENCY_आवरणम्);
				Const orig_cx_आवरणम् = पत्तनम्.cell->parameters.at(ID::COLLISION_X_आवरणम्);
				प्रति (पूर्णाङ्क orig_portid : wr_portid) {
					पत्तनम्.transparency_आवरणम्.प्रक्षिप_पृष्ठभागः(orig_portid < Get_आकार(orig_trans_आवरणम्) && orig_trans_आवरणम्[orig_portid] == State::S1);
					पत्तनम्.collision_x_आवरणम्.प्रक्षिप_पृष्ठभागः(orig_portid < Get_आकार(orig_cx_आवरणम्) && orig_cx_आवरणम्[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		प्रत्यागम res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().सङ्कलय(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::विस्तारः_2).as_int(),
			cell->parameters.at(ID::विस्थापनम्_2).as_int(),
			cell->parameters.at(ID::आकार_2).as_int()
		);
		तार्किक is_compat = cell->type == ID($mem);
		पूर्णाङ्क abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = सत्य;
		res.cell = cell;
		res.गुणाः = cell->गुणाः;
		Const &init = cell->parameters.at(ID::INIT);
		यदि (!init.is_fully_undef()) {
			पूर्णाङ्क pos = 0;
			यावत् (pos < res.आकार) {
				Const word = init.extract(pos * res.विस्तारः, res.विस्तारः, State::Sx);
				यदि (word.is_fully_undef()) {
					pos++;
				} अन्यथा {
					पूर्णाङ्क epos;
					प्रति (epos = pos; epos < res.आकार; epos++) {
						Const eword = init.extract(epos * res.विस्तारः, res.विस्तारः, State::Sx);
						यदि (eword.is_fully_undef())
							भङ्ग;
					}
					MemInit minit;
					minit.addr = res.start_विस्थापनम् + pos;
					minit.दत्त = init.extract(pos * res.विस्तारः, (epos - pos) * res.विस्तारः, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.विस्तारः);
					res.inits.प्रक्षिप_पृष्ठभागः(minit);
					pos = epos;
				}
			}
		}
		पूर्णाङ्क n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		पूर्णाङ्क n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		प्रति (पूर्णाङ्क i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			यावत् (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).extract(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).extract(i, 1).as_bool();
			mrd.clk = cell->get_पत्तनम्(ID::RD_CLK).extract(i, 1);
			mrd.en = cell->get_पत्तनम्(ID::RD_EN).extract(i, 1);
			mrd.addr = cell->get_पत्तनम्(ID::RD_ADDR).extract(i * abits, abits);
			mrd.दत्त = cell->get_पत्तनम्(ID::RD_दत्त).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
			यदि (is_compat) {
				mrd.ce_over_srst = असत्य;
				mrd.arst_मानम् = Const(State::Sx, res.विस्तारः << mrd.wide_log2);
				mrd.srst_मानम् = Const(State::Sx, res.विस्तारः << mrd.wide_log2);
				mrd.init_मानम् = Const(State::Sx, res.विस्तारः << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} अन्यथा {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).extract(i, 1).as_bool();
				mrd.arst_मानम् = cell->parameters.at(ID::RD_ARST_मानम्).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
				mrd.srst_मानम् = cell->parameters.at(ID::RD_SRST_मानम्).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
				mrd.init_मानम् = cell->parameters.at(ID::RD_INIT_मानम्).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
				mrd.arst = cell->get_पत्तनम्(ID::RD_ARST).extract(i, 1);
				mrd.srst = cell->get_पत्तनम्(ID::RD_SRST).extract(i, 1);
			}
			यदि (!is_compat) {
				Const transparency_आवरणम् = cell->parameters.at(ID::RD_TRANSPARENCY_आवरणम्).extract(i * n_wr_ports, n_wr_ports);
				Const collision_x_आवरणम् = cell->parameters.at(ID::RD_COLLISION_X_आवरणम्).extract(i * n_wr_ports, n_wr_ports);
				प्रति (पूर्णाङ्क j = 0; j < n_wr_ports; j++)
					यदि (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_आवरणम्.प्रक्षिप_पृष्ठभागः(transparency_आवरणम्[j] == State::S1);
						mrd.collision_x_आवरणम्.प्रक्षिप_पृष्ठभागः(collision_x_आवरणम्[j] == State::S1);
					}
			}
			res.rd_ports.प्रक्षिप_पृष्ठभागः(mrd);
		}
		प्रति (पूर्णाङ्क i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			यावत् (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).extract(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).extract(i, 1).as_bool();
			mwr.clk = cell->get_पत्तनम्(ID::WR_CLK).extract(i, 1);
			mwr.en = cell->get_पत्तनम्(ID::WR_EN).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
			mwr.addr = cell->get_पत्तनम्(ID::WR_ADDR).extract(i * abits, abits);
			mwr.दत्त = cell->get_पत्तनम्(ID::WR_दत्त).extract(i * res.विस्तारः, (ni - i) * res.विस्तारः);
			यदि (!is_compat) {
				Const प्राथमिकता_आवरणम् = cell->parameters.at(ID::WR_प्राथमिकता_आवरणम्).extract(i * n_wr_ports, n_wr_ports);
				प्रति (पूर्णाङ्क j = 0; j < n_wr_ports; j++)
					यदि (wr_wide_continuation[j] != State::S1)
						mwr.प्राथमिकता_आवरणम्.प्रक्षिप_पृष्ठभागः(प्राथमिकता_आवरणम्[j] == State::S1);
			}
			res.wr_ports.प्रक्षिप_पृष्ठभागः(mwr);
		}
		यदि (is_compat) {
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(res.wr_ports); i++) {
				स्वचालित &पत्तनम् = res.wr_ports[i];
				पत्तनम्.प्राथमिकता_आवरणम्.resize(Get_आकार(res.wr_ports));
				प्रति (पूर्णाङ्क j = 0; j < i; j++) {
					स्वचालित &oport = res.wr_ports[j];
					यदि (पत्तनम्.clk_enable != oport.clk_enable)
						अनुवर्त;
					यदि (पत्तनम्.clk_enable && पत्तनम्.clk != oport.clk)
						अनुवर्त;
					यदि (पत्तनम्.clk_enable && पत्तनम्.clk_polarity != oport.clk_polarity)
						अनुवर्त;
					पत्तनम्.प्राथमिकता_आवरणम्[j] = सत्य;
				}
			}
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(res.rd_ports); i++) {
				स्वचालित &पत्तनम् = res.rd_ports[i];
				पत्तनम्.transparency_आवरणम्.resize(Get_आकार(res.wr_ports));
				पत्तनम्.collision_x_आवरणम्.resize(Get_आकार(res.wr_ports));
				यदि (!cell->parameters.at(ID::RD_TRANSPARENT).extract(i, 1).as_bool())
					अनुवर्त;
				यदि (!पत्तनम्.clk_enable)
					अनुवर्त;
				प्रति (पूर्णाङ्क j = 0; j < Get_आकार(res.wr_ports); j++) {
					स्वचालित &wport = res.wr_ports[j];
					यदि (!wport.clk_enable)
						अनुवर्त;
					यदि (पत्तनम्.clk != wport.clk)
						अनुवर्त;
					यदि (पत्तनम्.clk_polarity != wport.clk_polarity)
						अनुवर्त;
					पत्तनम्.transparency_आवरणम्[j] = सत्य;
				}
			}
		}
		res.check();
		प्रत्यागम res;
	}

}

std::सदिशम्<Mem> Mem::get_all_memories(Module *module) {
	std::सदिशम्<Mem> res;
	Mem_सूचकांक सूचकांक(module);
	प्रति (स्वचालित it: module->memories) {
		res.प्रक्षिप_पृष्ठभागः(mem_from_स्मृतिः(module, it.second, सूचकांक));
	}
	प्रति (स्वचालित cell: module->cells()) {
		यदि (cell->type.अन्तर्(ID($mem), ID($mem_v2)))
			res.प्रक्षिप_पृष्ठभागः(mem_from_cell(cell));
	}
	प्रत्यागम res;
}

std::सदिशम्<Mem> Mem::get_selected_memories(Module *module) {
	std::सदिशम्<Mem> res;
	Mem_सूचकांक सूचकांक(module);
	प्रति (स्वचालित it: module->memories) {
		यदि (module->design->selected(module, it.second))
			res.प्रक्षिप_पृष्ठभागः(mem_from_स्मृतिः(module, it.second, सूचकांक));
	}
	प्रति (स्वचालित cell: module->selected_cells()) {
		यदि (cell->type.अन्तर्(ID($mem), ID($mem_v2)))
			res.प्रक्षिप_पृष्ठभागः(mem_from_cell(cell));
	}
	प्रत्यागम res;
}

Cell *Mem::extract_rdff(पूर्णाङ्क idx, FfInitVals *initvals) {
	MemRd &पत्तनम् = rd_ports[idx];

	यदि (!पत्तनम्.clk_enable)
		प्रत्यागम शून्यसूचकः;

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
	तार्किक trans_use_addr = सत्य;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++)
		यदि (!पत्तनम्.transparency_आवरणम्[i] && !wr_ports[i].removed)
			trans_use_addr = असत्य;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	यदि (Get_आकार(wr_ports) == 0)
		trans_use_addr = असत्य;

	यदि (पत्तनम्.en != State::S1 || पत्तनम्.srst != State::S0 || पत्तनम्.arst != State::S0 || !पत्तनम्.init_मानम्.is_fully_undef())
		trans_use_addr = असत्य;

	यदि (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		पूर्णाङ्क विस्तारः = 0;
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(पत्तनम्.addr); i++)
			यदि (पत्तनम्.addr[i].wire)
				विस्तारः++;

		यदि (विस्तारः)
		{
			SigSpec sig_q = module->सङ्कलय_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), विस्तारः);
			SigSpec sig_d;

			पूर्णाङ्क pos = 0;
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(पत्तनम्.addr); i++)
				यदि (पत्तनम्.addr[i].wire) {
					sig_d.append(पत्तनम्.addr[i]);
					पत्तनम्.addr[i] = sig_q[pos++];
				}

			c = module->सङ्कलय_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), पत्तनम्.clk, sig_d, sig_q, पत्तनम्.clk_polarity);
		} अन्यथा {
			c = शून्यसूचकः;
		}
	}
	अन्यथा
	{
		log_assert(पत्तनम्.arst == State::S0 || पत्तनम्.srst == State::S0);

		SigSpec async_d = module->सङ्कलय_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_आकार(पत्तनम्.दत्त));
		SigSpec sig_d = async_d;

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
			स्वचालित &wport = wr_ports[i];
			यदि (wport.removed)
				अनुवर्त;
			यदि (पत्तनम्.transparency_आवरणम्[i] || पत्तनम्.collision_x_आवरणम्[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == पत्तनम्.clk);
				log_assert(wport.clk_enable == पत्तनम्.clk_enable);
				पूर्णाङ्क min_wide_log2 = std::min(पत्तनम्.wide_log2, wport.wide_log2);
				पूर्णाङ्क max_wide_log2 = std::max(पत्तनम्.wide_log2, wport.wide_log2);
				तार्किक wide_write = wport.wide_log2 > पत्तनम्.wide_log2;
				प्रति (पूर्णाङ्क sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = पत्तनम्.addr;
					SigSpec waddr = wport.addr;
					यदि (wide_write)
						waddr = wport.sub_addr(sub);
					अन्यथा
						raddr = पत्तनम्.sub_addr(sub);
					SigSpec addr_eq;
					यदि (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					पूर्णाङ्क pos = 0;
					पूर्णाङ्क ewidth = विस्तारः << min_wide_log2;
					पूर्णाङ्क wsub = wide_write ? sub : 0;
					पूर्णाङ्क rsub = wide_write ? 0 : sub;
					यावत् (pos < ewidth) {
						पूर्णाङ्क epos = pos;
						यावत् (epos < ewidth && wport.en[epos + wsub * विस्तारः] == wport.en[pos + wsub * विस्तारः])
							epos++;
						SigSpec cur = sig_d.extract(pos + rsub * विस्तारः, epos-pos);
						SigSpec other = पत्तनम्.transparency_आवरणम्[i] ? wport.दत्त.extract(pos + wsub * विस्तारः, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						यदि (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * विस्तारः], addr_eq);
						अन्यथा
							cond = wport.en[pos + wsub * विस्तारः];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * विस्तारः, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().सङ्कलय(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_दत्त ff(module, initvals, name);
		ff.विस्तारः = Get_आकार(पत्तनम्.दत्त);
		ff.has_clk = सत्य;
		ff.sig_clk = पत्तनम्.clk;
		ff.pol_clk = पत्तनम्.clk_polarity;
		यदि (पत्तनम्.en != State::S1) {
			ff.has_ce = सत्य;
			ff.pol_ce = सत्य;
			ff.sig_ce = पत्तनम्.en;
		}
		यदि (पत्तनम्.arst != State::S0) {
			ff.has_arst = सत्य;
			ff.pol_arst = सत्य;
			ff.sig_arst = पत्तनम्.arst;
			ff.val_arst = पत्तनम्.arst_मानम्;
		}
		यदि (पत्तनम्.srst != State::S0) {
			ff.has_srst = सत्य;
			ff.pol_srst = सत्य;
			ff.sig_srst = पत्तनम्.srst;
			ff.val_srst = पत्तनम्.srst_मानम्;
			ff.ce_over_srst = ff.has_ce && पत्तनम्.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = पत्तनम्.दत्त;
		ff.val_init = पत्तनम्.init_मानम्;
		पत्तनम्.दत्त = async_d;
		c = ff.emit();
	}

	यदि (c)
		लघुगणकः("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	पत्तनम्.en = State::S1;
	पत्तनम्.clk = State::S0;
	पत्तनम्.arst = State::S0;
	पत्तनम्.srst = State::S0;
	पत्तनम्.clk_enable = असत्य;
	पत्तनम्.clk_polarity = सत्य;
	पत्तनम्.ce_over_srst = असत्य;
	पत्तनम्.arst_मानम् = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));
	पत्तनम्.srst_मानम् = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));
	पत्तनम्.init_मानम् = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));

	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
		पत्तनम्.transparency_आवरणम्[i] = असत्य;
		पत्तनम्.collision_x_आवरणम्[i] = असत्य;
	}

	प्रत्यागम c;
}

शून्य Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::सदिशम्<MemRd> new_rd_ports;
	std::सदिशम्<MemWr> new_wr_ports;
	std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> new_rd_map;
	std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> new_wr_map;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++) {
		स्वचालित &पत्तनम् = rd_ports[i];
		प्रति (पूर्णाङ्क sub = 0; sub < (1 << पत्तनम्.wide_log2); sub++) {
			new_rd_map.प्रक्षिप_पृष्ठभागः(std::make_pair(i, sub));
		}
	}
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
		स्वचालित &पत्तनम् = wr_ports[i];
		प्रति (पूर्णाङ्क sub = 0; sub < (1 << पत्तनम्.wide_log2); sub++) {
			new_wr_map.प्रक्षिप_पृष्ठभागः(std::make_pair(i, sub));
		}
	}
	प्रति (स्वचालित &it : new_rd_map) {
		MemRd &orig = rd_ports[it.first];
		MemRd पत्तनम् = orig;
		यदि (it.second != 0)
			पत्तनम्.cell = शून्यसूचकः;
		यदि (पत्तनम्.wide_log2) {
			पत्तनम्.दत्त = पत्तनम्.दत्त.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.init_मानम् = पत्तनम्.init_मानम्.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.arst_मानम् = पत्तनम्.arst_मानम्.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.srst_मानम् = पत्तनम्.srst_मानम्.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.addr = पत्तनम्.sub_addr(it.second);
			पत्तनम्.wide_log2 = 0;
		}
		पत्तनम्.transparency_आवरणम्.clear();
		पत्तनम्.collision_x_आवरणम्.clear();
		प्रति (स्वचालित &it2 : new_wr_map)
			पत्तनम्.transparency_आवरणम्.प्रक्षिप_पृष्ठभागः(orig.transparency_आवरणम्[it2.first]);
		प्रति (स्वचालित &it2 : new_wr_map)
			पत्तनम्.collision_x_आवरणम्.प्रक्षिप_पृष्ठभागः(orig.collision_x_आवरणम्[it2.first]);
		new_rd_ports.प्रक्षिप_पृष्ठभागः(पत्तनम्);
	}
	प्रति (स्वचालित &it : new_wr_map) {
		MemWr &orig = wr_ports[it.first];
		MemWr पत्तनम् = orig;
		यदि (it.second != 0)
			पत्तनम्.cell = शून्यसूचकः;
		यदि (पत्तनम्.wide_log2) {
			पत्तनम्.दत्त = पत्तनम्.दत्त.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.en = पत्तनम्.en.extract(it.second * विस्तारः, विस्तारः);
			पत्तनम्.addr = पत्तनम्.sub_addr(it.second);
			पत्तनम्.wide_log2 = 0;
		}
		पत्तनम्.प्राथमिकता_आवरणम्.clear();
		प्रति (स्वचालित &it2 : new_wr_map)
			पत्तनम्.प्राथमिकता_आवरणम्.प्रक्षिप_पृष्ठभागः(orig.प्राथमिकता_आवरणम्[it2.first]);
		new_wr_ports.प्रक्षिप_पृष्ठभागः(पत्तनम्);
	}
	std::swap(rd_ports, new_rd_ports);
	std::swap(wr_ports, new_wr_ports);
}

शून्य Mem::emulate_प्राथमिकता(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals)
{
	स्वचालित &port1 = wr_ports[idx1];
	स्वचालित &port2 = wr_ports[idx2];
	यदि (!port2.प्राथमिकता_आवरणम्[idx1])
		प्रत्यागम;
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++) {
		स्वचालित &rport = rd_ports[i];
		यदि (rport.removed)
			अनुवर्त;
		यदि (rport.transparency_आवरणम्[idx1] && !(rport.transparency_आवरणम्[idx2] || rport.collision_x_आवरणम्[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	पूर्णाङ्क min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	पूर्णाङ्क max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	तार्किक wide1 = port1.wide_log2 > port2.wide_log2;
	प्रति (पूर्णाङ्क sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		यदि (wide1)
			addr1 = port1.sub_addr(sub);
		अन्यथा
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		पूर्णाङ्क ewidth = विस्तारः << min_wide_log2;
		पूर्णाङ्क sub1 = wide1 ? sub : 0;
		पूर्णाङ्क sub2 = wide1 ? 0 : sub;
		dict<std::pair<SigBit, SigBit>, SigBit> cache;
		प्रति (पूर्णाङ्क pos = 0; pos < ewidth; pos++) {
			SigBit &en1 = port1.en[pos + sub1 * विस्तारः];
			SigBit &en2 = port2.en[pos + sub2 * विस्तारः];
			std::pair<SigBit, SigBit> कुञ्चिका(en1, en2);
			यदि (cache.गणनम्(कुञ्चिका)) {
				en1 = cache[कुञ्चिका];
			} अन्यथा {
				SigBit active2 = module->And(NEW_ID, addr_eq, en2);
				SigBit nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[कुञ्चिका] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.प्राथमिकता_आवरणम्[idx1] = असत्य;
}

शून्य Mem::emulate_transparency(पूर्णाङ्क widx, पूर्णाङ्क ridx, FfInitVals *initvals) {
	स्वचालित &wport = wr_ports[widx];
	स्वचालित &rport = rd_ports[ridx];
	log_assert(rport.transparency_आवरणम्[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	प्रति (पूर्णाङ्क i = Get_आकार(wr_ports) - 1; i > widx; i--) {
		यदि (wr_ports[i].removed)
			अनुवर्त;
		यदि (rport.transparency_आवरणम्[i] && wr_ports[i].प्राथमिकता_आवरणम्[widx])
			emulate_transparency(i, ridx, initvals);
	}
	पूर्णाङ्क min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	पूर्णाङ्क max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	तार्किक wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->सङ्कलय_Wire(NEW_ID, Get_आकार(wport.दत्त));
	module->सङ्कलय_Dffe(NEW_ID, rport.clk, rport.en, wport.दत्त, wdata_q, rport.clk_polarity, सत्य);
	प्रति (पूर्णाङ्क sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		प्रति (पूर्णाङ्क j = min_wide_log2; j < max_wide_log2; j++)
			यदि (wide_write)
				waddr = wport.sub_addr(sub);
			अन्यथा
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		यदि (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		पूर्णाङ्क pos = 0;
		पूर्णाङ्क ewidth = विस्तारः << min_wide_log2;
		पूर्णाङ्क wsub = wide_write ? sub : 0;
		पूर्णाङ्क rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->सङ्कलय_Wire(NEW_ID, ewidth);
		यावत् (pos < ewidth) {
			पूर्णाङ्क epos = pos;
			यावत् (epos < ewidth && wport.en[epos + wsub * विस्तारः] == wport.en[pos + wsub * विस्तारः])
				epos++;
			SigSpec cond;
			यदि (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * विस्तारः], addr_eq);
			अन्यथा
				cond = wport.en[pos + wsub * विस्तारः];
			SigSpec cond_q = module->सङ्कलय_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_दत्त ff(module, initvals, NEW_ID);
			ff.विस्तारः = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = सत्य;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			यदि (rport.en != State::S1) {
				ff.has_ce = सत्य;
				ff.sig_ce = rport.en;
				ff.pol_ce = सत्य;
			}
			यदि (rport.arst != State::S0) {
				ff.has_arst = सत्य;
				ff.sig_arst = rport.arst;
				ff.pol_arst = सत्य;
				ff.val_arst = State::S0;
			}
			यदि (rport.srst != State::S0) {
				ff.has_srst = सत्य;
				ff.sig_srst = rport.srst;
				ff.pol_srst = सत्य;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			यदि (!rport.init_मानम्.is_fully_undef())
				ff.val_init = State::S0;
			अन्यथा
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.extract(pos, epos-pos);
			SigSpec other = wdata_q.extract(pos + wsub * विस्तारः, epos-pos);
			SigSpec dest = rport.दत्त.extract(pos + rsub * विस्तारः, epos-pos);
			module->सङ्कलय_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.दत्त.replace(rsub * विस्तारः, rdata_a);
	}
	rport.transparency_आवरणम्[widx] = असत्य;
	rport.collision_x_आवरणम्[widx] = सत्य;
}

शून्य Mem::prepare_wr_merge(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	स्वचालित &port1 = wr_ports[idx1];
	स्वचालित &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	प्रति (पूर्णाङ्क i = 0; i < idx1; i++)
		यदि (port2.प्राथमिकता_आवरणम्[i])
			port1.प्राथमिकता_आवरणम्[i] = सत्य;
	// If port 2 has priority over a port after port 1, emulate it.
	प्रति (पूर्णाङ्क i = idx1 + 1; i < idx2; i++)
		यदि (port2.प्राथमिकता_आवरणम्[i] && !wr_ports[i].removed)
			emulate_प्राथमिकता(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	प्रति (पूर्णाङ्क i = idx2 + 1; i < Get_आकार(wr_ports); i++) {
		स्वचालित &oport = wr_ports[i];
		यदि (oport.प्राथमिकता_आवरणम्[idx2])
			oport.प्राथमिकता_आवरणम्[idx1] = सत्य;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++) {
		स्वचालित &rport = rd_ports[i];
		यदि (rport.removed)
			अनुवर्त;
		// If collision already undefined with both ports, it's fine.
		यदि (rport.collision_x_आवरणम्[idx1] && rport.collision_x_आवरणम्[idx2])
			अनुवर्त;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		यदि (rport.collision_x_आवरणम्[idx1]) {
			rport.collision_x_आवरणम्[idx1] = असत्य;
			rport.transparency_आवरणम्[idx1] = rport.transparency_आवरणम्[idx2];
			अनुवर्त;
		}
		यदि (rport.collision_x_आवरणम्[idx2]) {
			rport.collision_x_आवरणम्[idx2] = असत्य;
			rport.transparency_आवरणम्[idx2] = rport.transparency_आवरणम्[idx1];
			अनुवर्त;
		}
		// If transparent with both ports, also fine.
		यदि (rport.transparency_आवरणम्[idx1] && rport.transparency_आवरणम्[idx2])
			अनुवर्त;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		यदि (rport.transparency_आवरणम्[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_आवरणम्[idx1] = असत्य;
			अनुवर्त;
		}
		यदि (rport.transparency_आवरणम्[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_आवरणम्[idx2] = असत्य;
			अनुवर्त;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

शून्य Mem::prepare_rd_merge(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals) {
	स्वचालित &port1 = rd_ports[idx1];
	स्वचालित &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wr_ports); i++) {
		यदि (wr_ports[i].removed)
			अनुवर्त;
		// Both ports undefined, OK.
		यदि (port1.collision_x_आवरणम्[i] && port2.collision_x_आवरणम्[i])
			अनुवर्त;
		// Only one port undefined — change its behavior
		// to align with the other port.
		यदि (port1.collision_x_आवरणम्[i]) {
			port1.collision_x_आवरणम्[i] = असत्य;
			port1.transparency_आवरणम्[i] = port2.transparency_आवरणम्[i];
			अनुवर्त;
		}
		यदि (port2.collision_x_आवरणम्[i]) {
			port2.collision_x_आवरणम्[i] = असत्य;
			port2.transparency_आवरणम्[i] = port1.transparency_आवरणम्[i];
			अनुवर्त;
		}
		// Both ports transparent, OK.
		यदि (port1.transparency_आवरणम्[i] && port2.transparency_आवरणम्[i])
			अनुवर्त;
		// Only one port transparent — emulate transparency
		// on the other.
		यदि (port1.transparency_आवरणम्[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_आवरणम्[i] = असत्य;
			अनुवर्त;
		}
		यदि (port2.transparency_आवरणम्[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_आवरणम्[i] = असत्य;
			अनुवर्त;
		}
		// No ports transparent, OK.
	}

}

शून्य Mem::widen_prep(पूर्णाङ्क wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	पूर्णाङ्क आवरणम् = ((1 << wide_log2) - 1);
	पूर्णाङ्क delta = start_विस्थापनम् & आवरणम्;
	start_विस्थापनम् -= delta;
	आकार += delta;
	यदि (आकार & आवरणम्) {
		आकार |= आवरणम्;
		आकार++;
	}
}

शून्य Mem::widen_wr_पत्तनम्(पूर्णाङ्क idx, पूर्णाङ्क wide_log2) {
	widen_prep(wide_log2);
	स्वचालित &पत्तनम् = wr_ports[idx];
	log_assert(पत्तनम्.wide_log2 <= wide_log2);
	यदि (पत्तनम्.wide_log2 < wide_log2) {
		SigSpec new_दत्त, new_en;
		SigSpec addr_lo = पत्तनम्.addr.extract(0, wide_log2);
		प्रति (पूर्णाङ्क sub = 0; sub < (1 << wide_log2); sub += (1 << पत्तनम्.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			यदि (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_दत्त.append(पत्तनम्.दत्त);
				new_en.append(पत्तनम्.en);
			} अन्यथा यदि (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_दत्त.append(Const(State::Sx, Get_आकार(पत्तनम्.दत्त)));
				new_en.append(Const(State::S0, Get_आकार(पत्तनम्.दत्त)));
			} अन्यथा {
				// May or may not write to this subword.
				new_दत्त.append(पत्तनम्.दत्त);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_आकार(पत्तनम्.दत्त)), पत्तनम्.en, addr_eq);
				new_en.append(en);
			}
		}
		पत्तनम्.addr.replace(पत्तनम्.wide_log2, Const(State::S0, wide_log2 - पत्तनम्.wide_log2));
		पत्तनम्.दत्त = new_दत्त;
		पत्तनम्.en = new_en;
		पत्तनम्.wide_log2 = wide_log2;
	}
}

शून्य Mem::emulate_rden(पूर्णाङ्क idx, FfInitVals *initvals) {
	स्वचालित &पत्तनम् = rd_ports[idx];
	log_assert(पत्तनम्.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
	Wire *पूर्व_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
	Wire *sel = module->सङ्कलय_Wire(NEW_ID);
	Ff_दत्त ff_sel(module, initvals, NEW_ID);
	Ff_दत्त ff_दत्त(module, initvals, NEW_ID);
	ff_sel.विस्तारः = 1;
	ff_sel.has_clk = सत्य;
	ff_sel.sig_clk = पत्तनम्.clk;
	ff_sel.pol_clk = पत्तनम्.clk_polarity;
	ff_sel.sig_d = पत्तनम्.en;
	ff_sel.sig_q = sel;
	ff_दत्त.विस्तारः = Get_आकार(पत्तनम्.दत्त);
	ff_दत्त.has_clk = सत्य;
	ff_दत्त.sig_clk = पत्तनम्.clk;
	ff_दत्त.pol_clk = पत्तनम्.clk_polarity;
	ff_दत्त.sig_d = पत्तनम्.दत्त;
	ff_दत्त.sig_q = पूर्व_दत्त;
	यदि (!पत्तनम्.init_मानम्.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_दत्त.val_init = पत्तनम्.init_मानम्;
		पत्तनम्.init_मानम् = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));
	} अन्यथा {
		ff_sel.val_init = State::Sx;
		ff_दत्त.val_init = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));
	}
	यदि (पत्तनम्.arst != State::S0) {
		ff_sel.has_arst = सत्य;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = पत्तनम्.arst;
		ff_sel.pol_arst = सत्य;
		ff_दत्त.has_arst = सत्य;
		ff_दत्त.val_arst = पत्तनम्.arst_मानम्;
		ff_दत्त.sig_arst = पत्तनम्.arst;
		ff_दत्त.pol_arst = सत्य;
		पत्तनम्.arst = State::S0;
	}
	यदि (पत्तनम्.srst != State::S0) {
		log_assert(!पत्तनम्.ce_over_srst);
		ff_sel.has_srst = सत्य;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = पत्तनम्.srst;
		ff_sel.pol_srst = सत्य;
		ff_sel.ce_over_srst = असत्य;
		ff_दत्त.has_srst = सत्य;
		ff_दत्त.val_srst = पत्तनम्.srst_मानम्;
		ff_दत्त.sig_srst = पत्तनम्.srst;
		ff_दत्त.pol_srst = सत्य;
		ff_दत्त.ce_over_srst = असत्य;
		पत्तनम्.srst = State::S0;
	}
	ff_sel.emit();
	ff_दत्त.emit();
	module->सङ्कलय_Mux(NEW_ID, पूर्व_दत्त, new_दत्त, sel, पत्तनम्.दत्त);
	पत्तनम्.दत्त = new_दत्त;
	पत्तनम्.en = State::S1;
}

शून्य Mem::emulate_reset(पूर्णाङ्क idx, तार्किक emu_init, तार्किक emu_arst, तार्किक emu_srst, FfInitVals *initvals) {
	स्वचालित &पत्तनम् = rd_ports[idx];
	यदि (emu_init && !पत्तनम्.init_मानम्.is_fully_undef()) {
		Wire *sel = module->सङ्कलय_Wire(NEW_ID);
		Ff_दत्त ff_sel(module, initvals, NEW_ID);
		Wire *new_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
		ff_sel.विस्तारः = 1;
		ff_sel.has_clk = सत्य;
		ff_sel.sig_clk = पत्तनम्.clk;
		ff_sel.pol_clk = पत्तनम्.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		यदि (पत्तनम्.en != State::S1) {
			ff_sel.has_ce = सत्य;
			ff_sel.sig_ce = पत्तनम्.en;
			ff_sel.pol_ce = सत्य;
			ff_sel.ce_over_srst = पत्तनम्.ce_over_srst;
		}
		यदि (पत्तनम्.arst != State::S0) {
			ff_sel.has_arst = सत्य;
			ff_sel.sig_arst = पत्तनम्.arst;
			ff_sel.pol_arst = सत्य;
			यदि (emu_arst && पत्तनम्.arst_मानम् == पत्तनम्.init_मानम्) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				पत्तनम्.arst = State::S0;
			} अन्यथा {
				ff_sel.val_arst = State::S1;
			}
		}
		यदि (पत्तनम्.srst != State::S0) {
			ff_sel.has_srst = सत्य;
			ff_sel.sig_srst = पत्तनम्.srst;
			ff_sel.pol_srst = सत्य;
			यदि (emu_srst && पत्तनम्.srst_मानम् == पत्तनम्.init_मानम्) {
				ff_sel.val_srst = State::S0;
				पत्तनम्.srst = State::S0;
			} अन्यथा {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->सङ्कलय_Mux(NEW_ID, पत्तनम्.init_मानम्, new_दत्त, sel, पत्तनम्.दत्त);
		पत्तनम्.दत्त = new_दत्त;
		पत्तनम्.init_मानम् = Const(State::Sx, Get_आकार(पत्तनम्.दत्त));
	}
	यदि (emu_arst && पत्तनम्.arst != State::S0) {
		Wire *sel = module->सङ्कलय_Wire(NEW_ID);
		Ff_दत्त ff_sel(module, initvals, NEW_ID);
		Wire *new_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
		ff_sel.विस्तारः = 1;
		ff_sel.has_clk = सत्य;
		ff_sel.sig_clk = पत्तनम्.clk;
		ff_sel.pol_clk = पत्तनम्.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		यदि (पत्तनम्.init_मानम्.is_fully_undef())
			ff_sel.val_init = State::Sx;
		अन्यथा
			ff_sel.val_init = State::S1;
		यदि (पत्तनम्.en != State::S1) {
			ff_sel.has_ce = सत्य;
			ff_sel.sig_ce = पत्तनम्.en;
			ff_sel.pol_ce = सत्य;
			ff_sel.ce_over_srst = पत्तनम्.ce_over_srst;
		}
		ff_sel.has_arst = सत्य;
		ff_sel.sig_arst = पत्तनम्.arst;
		ff_sel.pol_arst = सत्य;
		ff_sel.val_arst = State::S0;
		यदि (पत्तनम्.srst != State::S0) {
			ff_sel.has_srst = सत्य;
			ff_sel.sig_srst = पत्तनम्.srst;
			ff_sel.pol_srst = सत्य;
			यदि (emu_srst && पत्तनम्.srst_मानम् == पत्तनम्.arst_मानम्) {
				ff_sel.val_srst = State::S0;
				पत्तनम्.srst = State::S0;
			} अन्यथा {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->सङ्कलय_Mux(NEW_ID, पत्तनम्.arst_मानम्, new_दत्त, sel, पत्तनम्.दत्त);
		पत्तनम्.दत्त = new_दत्त;
		पत्तनम्.arst = State::S0;
	}
	यदि (emu_srst && पत्तनम्.srst != State::S0) {
		Wire *sel = module->सङ्कलय_Wire(NEW_ID);
		Ff_दत्त ff_sel(module, initvals, NEW_ID);
		Wire *new_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
		ff_sel.विस्तारः = 1;
		ff_sel.has_clk = सत्य;
		ff_sel.sig_clk = पत्तनम्.clk;
		ff_sel.pol_clk = पत्तनम्.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		यदि (पत्तनम्.init_मानम्.is_fully_undef())
			ff_sel.val_init = State::Sx;
		अन्यथा
			ff_sel.val_init = State::S1;
		यदि (पत्तनम्.en != State::S1) {
			ff_sel.has_ce = सत्य;
			ff_sel.sig_ce = पत्तनम्.en;
			ff_sel.pol_ce = सत्य;
			ff_sel.ce_over_srst = पत्तनम्.ce_over_srst;
		}
		ff_sel.has_srst = सत्य;
		ff_sel.sig_srst = पत्तनम्.srst;
		ff_sel.pol_srst = सत्य;
		ff_sel.val_srst = State::S0;
		यदि (पत्तनम्.arst != State::S0) {
			ff_sel.has_arst = सत्य;
			ff_sel.sig_arst = पत्तनम्.arst;
			ff_sel.pol_arst = सत्य;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->सङ्कलय_Mux(NEW_ID, पत्तनम्.srst_मानम्, new_दत्त, sel, पत्तनम्.दत्त);
		पत्तनम्.दत्त = new_दत्त;
		पत्तनम्.srst = State::S0;
	}
}

शून्य Mem::emulate_rd_ce_over_srst(पूर्णाङ्क idx) {
	स्वचालित &पत्तनम् = rd_ports[idx];
	log_assert(पत्तनम्.clk_enable);
	यदि (पत्तनम्.en == State::S1 || पत्तनम्.srst == State::S0 || !पत्तनम्.ce_over_srst) {
		पत्तनम्.ce_over_srst = असत्य;
		प्रत्यागम;
	}
	पत्तनम्.ce_over_srst = असत्य;
	पत्तनम्.srst = module->And(NEW_ID, पत्तनम्.en, पत्तनम्.srst);
}

शून्य Mem::emulate_rd_srst_over_ce(पूर्णाङ्क idx) {
	स्वचालित &पत्तनम् = rd_ports[idx];
	log_assert(पत्तनम्.clk_enable);
	यदि (पत्तनम्.en == State::S1 || पत्तनम्.srst == State::S0 || पत्तनम्.ce_over_srst) {
		पत्तनम्.ce_over_srst = सत्य;
		प्रत्यागम;
	}
	पत्तनम्.ce_over_srst = सत्य;
	पत्तनम्.en = module->Or(NEW_ID, पत्तनम्.en, पत्तनम्.srst);
}

तार्किक Mem::emulate_read_first_ok() {
	यदि (wr_ports.रिक्तानि())
		प्रत्यागम असत्य;
	SigSpec clk = wr_ports[0].clk;
	तार्किक clk_polarity = wr_ports[0].clk_polarity;
	प्रति (स्वचालित &पत्तनम्: wr_ports) {
		यदि (!पत्तनम्.clk_enable)
			प्रत्यागम असत्य;
		यदि (पत्तनम्.clk != clk)
			प्रत्यागम असत्य;
		यदि (पत्तनम्.clk_polarity != clk_polarity)
			प्रत्यागम असत्य;
	}
	तार्किक प्राप्तम्_read_first = असत्य;
	प्रति (स्वचालित &पत्तनम्: rd_ports) {
		यदि (!पत्तनम्.clk_enable)
			प्रत्यागम असत्य;
		यदि (पत्तनम्.clk != clk)
			प्रत्यागम असत्य;
		यदि (पत्तनम्.clk_polarity != clk_polarity)
			प्रत्यागम असत्य;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		प्रति (पूर्णाङ्क j = 0; j < Get_आकार(wr_ports); j++)
			यदि (!पत्तनम्.transparency_आवरणम्[j] && !पत्तनम्.collision_x_आवरणम्[j])
				प्राप्तम्_read_first = सत्य;
	}
	प्रत्यागम प्राप्तम्_read_first;
}

शून्य Mem::emulate_read_first(FfInitVals *initvals) {
	log_assert(emulate_read_first_ok());
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++)
		प्रति (पूर्णाङ्क j = 0; j < Get_आकार(wr_ports); j++)
			यदि (rd_ports[i].transparency_आवरणम्[j])
				emulate_transparency(j, i, initvals);
	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(rd_ports); i++)
		प्रति (पूर्णाङ्क j = 0; j < Get_आकार(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_आवरणम्[j]);
			rd_ports[i].collision_x_आवरणम्[j] = असत्य;
			rd_ports[i].transparency_आवरणम्[j] = सत्य;
		}
	प्रति (स्वचालित &पत्तनम्: wr_ports) {
		Wire *new_दत्त = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.दत्त));
		Wire *new_addr = module->सङ्कलय_Wire(NEW_ID, Get_आकार(पत्तनम्.addr));
		स्वचालित compressed = पत्तनम्.compress_en();
		Wire *new_en = module->सङ्कलय_Wire(NEW_ID, Get_आकार(compressed.first));
		Ff_दत्त ff_दत्त(module, initvals, NEW_ID);
		Ff_दत्त ff_addr(module, initvals, NEW_ID);
		Ff_दत्त ff_en(module, initvals, NEW_ID);
		ff_दत्त.विस्तारः = Get_आकार(पत्तनम्.दत्त);
		ff_दत्त.has_clk = सत्य;
		ff_दत्त.sig_clk = पत्तनम्.clk;
		ff_दत्त.pol_clk = पत्तनम्.clk_polarity;
		ff_दत्त.sig_d = पत्तनम्.दत्त;
		ff_दत्त.sig_q = new_दत्त;;
		ff_दत्त.val_init = Const(State::Sx, ff_दत्त.विस्तारः);
		ff_दत्त.emit();
		ff_addr.विस्तारः = Get_आकार(पत्तनम्.addr);
		ff_addr.has_clk = सत्य;
		ff_addr.sig_clk = पत्तनम्.clk;
		ff_addr.pol_clk = पत्तनम्.clk_polarity;
		ff_addr.sig_d = पत्तनम्.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.विस्तारः);
		ff_addr.emit();
		ff_en.विस्तारः = Get_आकार(compressed.first);
		ff_en.has_clk = सत्य;
		ff_en.sig_clk = पत्तनम्.clk;
		ff_en.pol_clk = पत्तनम्.clk_polarity;
		ff_en.sig_d = compressed.first;
		ff_en.sig_q = new_en;;
		यदि (inits.रिक्तानि())
			ff_en.val_init = Const(State::Sx, ff_en.विस्तारः);
		अन्यथा
			ff_en.val_init = Const(State::S0, ff_en.विस्तारः);
		ff_en.emit();
		पत्तनम्.दत्त = new_दत्त;
		पत्तनम्.addr = new_addr;
		पत्तनम्.en = पत्तनम्.decompress_en(compressed.second, new_en);
	}
}

std::pair<SigSpec, std::सदिशम्<पूर्णाङ्क>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::सदिशम्<पूर्णाङ्क> swizzle;
	SigBit पूर्व_bit = en[0];
	पूर्णाङ्क idx = 0;
	प्रति (स्वचालित &bit: en) {
		यदि (bit != पूर्व_bit) {
			sig.append(bit);
			पूर्व_bit = bit;
			idx++;
		}
		swizzle.प्रक्षिप_पृष्ठभागः(idx);
	}
	log_assert(idx + 1 == Get_आकार(sig));
	प्रत्यागम {sig, swizzle};
}

SigSpec MemWr::decompress_en(ध्रुव std::सदिशम्<पूर्णाङ्क> &swizzle, SigSpec sig) {
	SigSpec res;
	प्रति (पूर्णाङ्क i: swizzle)
		res.append(sig[i]);
	प्रत्यागम res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->आकार), mem->विस्तारः)
{
	प्रति(ध्रुव स्वचालित &init : mem->inits) {
		यदि(init.en.is_fully_zero()) अनुवर्त;
		log_assert(init.en.आकार() == दत्त_विस्तारः_2);
		यदि(init.en.is_fully_ones())
			निवेशय_concatenated(init.addr.as_int(), init.दत्त);
		अन्यथा {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t शब्दाः_2 = init.दत्त.आकार() / दत्त_विस्तारः_2;
			RTLIL::Const दत्त = init.दत्त;
			log_assert(दत्त.आकार() % दत्त_विस्तारः_2 == 0);
			प्रति(addr_t i = 0; i < शब्दाः_2; i++) {
				RTLIL::Const previous = (*एतत्)[addr + i];
				प्रति(पूर्णाङ्क j = 0; j < दत्त_विस्तारः_2; j++)
					यदि(init.en[j] != State::S1)
						दत्त.समुच्चयः(दत्त_विस्तारः_2 * i + j, previous[j]);
			}
			निवेशय_concatenated(init.addr.as_int(), दत्त);
		}
	}
}

MemContents::iterator & MemContents::iterator::संक्रियकः++() {
	स्वचालित it = स्मृतिः_2->मानानि_2.ऊर्ध्वस्थम्_bound(_addr);
	यदि(it == स्मृतिः_2->मानानि_2.अन्तः()) {
		स्मृतिः_2 = शून्यसूचकः;
		_addr = ~(addr_t) 0;
	} अन्यथा
		_addr = it->first;
	प्रत्यागम *एतत्;
}

शून्य MemContents::check() {
	log_assert(addr_विस्तारः_2 > 0 && addr_विस्तारः_2 < (पूर्णाङ्क)परिमाण(addr_t) * 8);
	log_assert(दत्त_विस्तारः_2 > 0);
	log_assert(default_मानम्.आकार() == दत्त_विस्तारः_2);
	यदि(मानानि_2.रिक्तानि()) प्रत्यागम;
	स्वचालित it = मानानि_2.begin();
	प्रति(;;) {
		log_assert(!it->second.रिक्तानि());
		log_assert(it->second.आकार() % दत्त_विस्तारः_2 == 0);
		स्वचालित end1 = range_अन्तः(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_विस्तारः_2));
		log_assert(end1 <= (addr_t)(1<<addr_विस्तारः_2));
		यदि(++it == मानानि_2.अन्तः())
			भङ्ग;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

तार्किक MemContents::range_अन्तर्भवति(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ध्रुव {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	प्रत्यागम it != मानानि_2.अन्तः() && addr - _range_begin(it) < range_आकार(it);
}


तार्किक MemContents::range_अन्तर्भवति(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t अन्तः_addr) ध्रुव {
	// note that we assume begin_addr <= end_addr
	प्रत्यागम it != मानानि_2.अन्तः() && _range_begin(it) <= begin_addr && अन्तः_addr - _range_begin(it) <= range_आकार(it);
}

तार्किक MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t अन्तः_addr) ध्रुव {
	यदि(it == मानानि_2.अन्तः() || begin_addr >= अन्तः_addr)
		प्रत्यागम असत्य;
	स्वचालित top1 = range_अन्तः(it) - 1;
	स्वचालित top2 = अन्तः_addr - 1;
	प्रत्यागम !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ध्रुव {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_विस्तारः_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	स्वचालित it = const_cast<std::map<addr_t, RTLIL::Const> &>(मानानि_2).ऊर्ध्वस्थम्_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	यदि(it == मानानि_2.begin())
		प्रत्यागम it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	स्वचालित it_पूर्व = std::अग्र(it, -1);
	यदि(range_अन्तर्भवति(it_पूर्व, addr))
		प्रत्यागम it_पूर्व;
	अन्यथा
		प्रत्यागम it;
}

RTLIL::Const MemContents::संक्रियकः[](addr_t addr) ध्रुव {
	स्वचालित it = _range_at(addr);
	यदि(range_अन्तर्भवति(it, addr))
		प्रत्यागम it->second.extract(range_विस्थापनम्(it, addr), दत्त_विस्तारः_2);
	अन्यथा
		प्रत्यागम default_मानम्;
}

MemContents::addr_t MemContents::गणनम्_range(addr_t begin_addr, addr_t अन्तः_addr) ध्रुव {
	addr_t गणनम् = 0;
	प्रति(स्वचालित it = _range_at(begin_addr); _range_overlaps(it, begin_addr, अन्तः_addr); it++) {
		स्वचालित first = std::max(_range_begin(it), begin_addr);
		स्वचालित चरमः = std::min(range_अन्तः(it), अन्तः_addr);
		गणनम् += चरमः - first;
	}
	प्रत्यागम गणनम्;
}

शून्य MemContents::clear_range(addr_t begin_addr, addr_t अन्तः_addr) {
	यदि(begin_addr >= अन्तः_addr) प्रत्यागम;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	स्वचालित begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	स्वचालित अन्तः_it = मानानि_2.ऊर्ध्वस्थम्_bound(अन्तः_addr - 1);
	यदि(begin_it == अन्तः_it)
		प्रत्यागम; // nothing to do
	// the last iterator affected is one before the first one not affected
	स्वचालित चरमः_it = std::अग्र(अन्तः_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	यदि(range_अन्तर्भवति(चरमः_it, अन्तः_addr - 1)) {
		स्वचालित new_begin = अन्तः_addr;
		स्वचालित अन्तः = range_अन्तः(चरमः_it);
		// if there is data past the end address, preserve it by creating a new range
		यदि(new_begin != अन्तः)
			अन्तः_it = मानानि_2.emplace_hint(चरमः_it, new_begin, चरमः_it->second.extract(range_विस्थापनम्(चरमः_it, new_begin), (range_अन्तः(चरमः_it) - new_begin) * दत्त_विस्तारः_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	यदि(range_अन्तर्भवति(begin_it, begin_addr)) {
		स्वचालित new_अन्तः = begin_addr;
		// if there is data before the start address, truncate but don't delete
		यदि(new_अन्तः != begin_it->first) {
			begin_it->second.extu(range_विस्थापनम्(begin_it, new_अन्तः));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	मानानि_2.erase(begin_it, अन्तः_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t अन्तः_addr) {
	यदि(begin_addr >= अन्तः_addr)
		प्रत्यागम मानानि_2.अन्तः(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	स्वचालित अधस्तनम्_it = begin_addr == 0 ? मानानि_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	यदि (range_अन्तर्भवति(अधस्तनम्_it, begin_addr, अन्तः_addr))
		प्रत्यागम अधस्तनम्_it;
	// find the first range containing any addr >= end_addr
	स्वचालित ऊर्ध्वस्थम्_it = _range_at(अन्तः_addr);
	// check if either of the two ranges we just found touch our range
	तार्किक अधस्तनम्_touch = begin_addr > 0 && range_अन्तर्भवति(अधस्तनम्_it, begin_addr - 1);
	तार्किक ऊर्ध्वस्थम्_touch = range_अन्तर्भवति(ऊर्ध्वस्थम्_it, अन्तः_addr);
	यदि (अधस्तनम्_touch && ऊर्ध्वस्थम्_touch) {
		log_assert (अधस्तनम्_it != ऊर्ध्वस्थम्_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		स्वचालित ऊर्ध्वस्थम्_अन्तः = range_अन्तः(ऊर्ध्वस्थम्_it);
		// make range bigger (maybe reserve here instead of resize?)
		अधस्तनम्_it->second.resize(range_विस्थापनम्(अधस्तनम्_it, ऊर्ध्वस्थम्_अन्तः), State::Sx);
		// copy only the data beyond our range
		std::copy(range_दत्त(ऊर्ध्वस्थम्_it, अन्तः_addr), range_दत्त(ऊर्ध्वस्थम्_it, ऊर्ध्वस्थम्_अन्तः), range_दत्त(अधस्तनम्_it, अन्तः_addr));
		// keep lower_it, but delete upper_it
		मानानि_2.erase(std::अग्र(अधस्तनम्_it), std::अग्र(ऊर्ध्वस्थम्_it));
		प्रत्यागम अधस्तनम्_it;
	} अन्यथा यदि (अधस्तनम्_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		अधस्तनम्_it->second.resize(range_विस्थापनम्(अधस्तनम्_it, अन्तः_addr), State::Sx);
		// keep lower_it and upper_it
		मानानि_2.erase(std::अग्र(अधस्तनम्_it), ऊर्ध्वस्थम्_it);
		प्रत्यागम अधस्तनम्_it;
	} अन्यथा यदि (ऊर्ध्वस्थम्_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const दत्त(State::Sx, (_range_begin(ऊर्ध्वस्थम्_it) - begin_addr) * दत्त_विस्तारः_2);
		दत्त.append(std::move(ऊर्ध्वस्थम्_it->second));
		// delete lower_it and upper_it, then reinsert
		मानानि_2.erase(अधस्तनम्_it, std::अग्र(ऊर्ध्वस्थम्_it));
		प्रत्यागम मानानि_2.emplace(begin_addr, std::move(दत्त)).first;
	} अन्यथा {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		मानानि_2.erase(अधस्तनम्_it, ऊर्ध्वस्थम्_it);
		प्रत्यागम मानानि_2.emplace(begin_addr, RTLIL::Const(State::Sx, (अन्तः_addr - begin_addr) * दत्त_विस्तारः_2)).first;
	}
}

शून्य MemContents::निवेशय_concatenated(addr_t addr, RTLIL::Const ध्रुव &मानानि) {
	addr_t शब्दाः_2 = (मानानि.आकार() + दत्त_विस्तारः_2 - 1) / दत्त_विस्तारः_2;
	log_assert(addr < (addr_t)(1<<addr_विस्तारः_2));
	log_assert(शब्दाः_2 <= (addr_t)(1<<addr_विस्तारः_2) - addr);
	स्वचालित it = _reserve_range(addr, addr + शब्दाः_2);
	स्वचालित गन्तव्यम्_begin = range_दत्त(it, addr);
	std::copy(मानानि.begin(), मानानि.अन्तः(), गन्तव्यम्_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(गन्तव्यम्_begin + मानानि.आकार(), गन्तव्यम्_begin + शब्दाः_2 * दत्त_विस्तारः_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ध्रुव &word) {
	स्वचालित from_अन्तः = word.आकार() <= दत्त_विस्तारः_2 ? word.अन्तः() : word.begin() + दत्त_विस्तारः_2;
	स्वचालित गन्तव्यम्_अन्तः = std::copy(word.begin(), from_अन्तः, it);
	स्वचालित it_अग्र = std::अग्र(it, दत्त_विस्तारः_2);
	std::fill(गन्तव्यम्_अन्तः, it_अग्र, State::S0);
	प्रत्यागम it_अग्र;
}
