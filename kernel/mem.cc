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

#شامل "kernel/mem.h"
#شامل "kernel/ff.h"

USING_YOSYS_NAMESPACE

خالی Mem::remove() {
	اگر (cell) {
		module->remove(cell);
		cell = خالی_اشاریہ;
	}
	اگر (mem) {
		module->memories.erase(mem->name);
		حذف mem;
		mem = خالی_اشاریہ;
	}
	برائے (خودکار &پورٹ : rd_ports) {
		اگر (پورٹ.cell) {
			module->remove(پورٹ.cell);
			پورٹ.cell = خالی_اشاریہ;
		}
	}
	برائے (خودکار &پورٹ : wr_ports) {
		اگر (پورٹ.cell) {
			module->remove(پورٹ.cell);
			پورٹ.cell = خالی_اشاریہ;
		}
	}
	برائے (خودکار &init : inits) {
		اگر (init.cell) {
			module->remove(init.cell);
			init.cell = خالی_اشاریہ;
		}
	}
}

خالی Mem::emit() {
	check();
	std::سمتیہ<صحیح> rd_بایاں;
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++) {
		خودکار &پورٹ = rd_ports[i];
		اگر (پورٹ.removed) {
			اگر (پورٹ.cell) {
				module->remove(پورٹ.cell);
			}
		} ورنہ {
			rd_بایاں.ڈالو_عقب(i);
		}
	}
	std::سمتیہ<صحیح> wr_بایاں;
	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		اگر (پورٹ.removed) {
			اگر (پورٹ.cell) {
				module->remove(پورٹ.cell);
			}
		} ورنہ {
			wr_بایاں.ڈالو_عقب(i);
		}
	}
	std::سمتیہ<صحیح> init_بایاں;
	برائے (صحیح i = 0; i < Get_سائز(inits); i++) {
		خودکار &init = inits[i];
		اگر (init.removed) {
			اگر (init.cell) {
				module->remove(init.cell);
			}
		} ورنہ {
			init_بایاں.ڈالو_عقب(i);
		}
	}
	برائے (صحیح i = 0; i < Get_سائز(rd_بایاں); i++)
		اگر (i != rd_بایاں[i])
			std::تبادلہ(rd_ports[i], rd_ports[rd_بایاں[i]]);
	rd_ports.resize(Get_سائز(rd_بایاں));
	برائے (صحیح i = 0; i < Get_سائز(wr_بایاں); i++)
		اگر (i != wr_بایاں[i])
			std::تبادلہ(wr_ports[i], wr_ports[wr_بایاں[i]]);
	wr_ports.resize(Get_سائز(wr_بایاں));
	برائے (صحیح i = 0; i < Get_سائز(init_بایاں); i++)
		اگر (i != init_بایاں[i])
			std::تبادلہ(inits[i], inits[init_بایاں[i]]);
	inits.resize(Get_سائز(init_بایاں));

	برائے (خودکار &پورٹ : rd_ports) {
		برائے (صحیح i = 0; i < Get_سائز(wr_بایاں); i++) {
			پورٹ.transparency_نقاب[i] = پورٹ.transparency_نقاب[wr_بایاں[i]];
			پورٹ.collision_x_نقاب[i] = پورٹ.collision_x_نقاب[wr_بایاں[i]];
		}
		پورٹ.transparency_نقاب.resize(Get_سائز(wr_بایاں));
		پورٹ.collision_x_نقاب.resize(Get_سائز(wr_بایاں));
	}
	برائے (خودکار &پورٹ : wr_ports) {
		برائے (صحیح i = 0; i < Get_سائز(wr_بایاں); i++)
			پورٹ.ترجیح_نقاب[i] = پورٹ.ترجیح_نقاب[wr_بایاں[i]];
		پورٹ.ترجیح_نقاب.resize(Get_سائز(wr_بایاں));
	}

	اگر (packed) {
		اگر (mem) {
			module->memories.erase(mem->name);
			حذف mem;
			mem = خالی_اشاریہ;
		}
		اگر (!cell) {
			اگر (memid.خالی_خانے())
				memid = PooledName(module->design, module->twines().جوڑو(NEW_ID));
			cell = module->جوڑو_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->اوصاف = اوصاف;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::چوڑائی_2] = Const(چوڑائی);
		cell->parameters[ID::انحراف_2] = Const(آغاز_انحراف);
		cell->parameters[ID::سائز_2] = Const(سائز);
		SigSpec rd_clk, rd_en, rd_addr, rd_ڈیٹا;
		SigSpec wr_clk, wr_en, wr_addr, wr_ڈیٹا;
		SigSpec rd_arst, rd_srst;
		صحیح abits = 0;
		برائے (خودکار &پورٹ : rd_ports)
			abits = std::max(abits, Get_سائز(پورٹ.addr));
		برائے (خودکار &پورٹ : wr_ports)
			abits = std::max(abits, Get_سائز(پورٹ.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::سمتیہ<صحیح> wr_پورٹ_xlat;
		برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++)
			برائے (صحیح j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_پورٹ_xlat.ڈالو_عقب(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_نقاب_builder;
		Const::Builder rd_collision_x_نقاب_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_قدر_builder;
		Const::Builder rd_srst_قدر_builder;
		Const::Builder rd_init_قدر_builder;
		برائے (خودکار &پورٹ : rd_ports) {
			برائے (خودکار attr: پورٹ.اوصاف)
				اگر (!cell->has_attribute(attr.پہلا))
					cell->اوصاف.داخل_کرو(attr);
			اگر (پورٹ.cell) {
				module->remove(پورٹ.cell);
				پورٹ.cell = خالی_اشاریہ;
			}
			برائے (صحیح sub = 0; sub < (1 << پورٹ.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ڈالو_عقب(State(sub != 0));
				rd_clk_enable_builder.ڈالو_عقب(State(پورٹ.clk_enable));
				rd_clk_polarity_builder.ڈالو_عقب(State(پورٹ.clk_polarity));
				rd_ce_over_srst_builder.ڈالو_عقب(State(پورٹ.ce_over_srst));
				rd_clk.append(پورٹ.clk);
				rd_arst.append(پورٹ.arst);
				rd_srst.append(پورٹ.srst);
				rd_en.append(پورٹ.en);
				SigSpec addr = پورٹ.sub_addr(sub);
				addr.extend_u0(abits, جھوٹ);
				rd_addr.append(addr);
				log_assert(Get_سائز(addr) == abits);
				برائے (خودکار idx : wr_پورٹ_xlat) {
					rd_transparency_نقاب_builder.ڈالو_عقب(State(منطقی(پورٹ.transparency_نقاب[idx])));
					rd_collision_x_نقاب_builder.ڈالو_عقب(State(منطقی(پورٹ.collision_x_نقاب[idx])));
				}
			}
			rd_ڈیٹا.append(پورٹ.ڈیٹا);
			برائے (خودکار ثنائی_ہندسہ : پورٹ.arst_قدر)
				rd_arst_قدر_builder.ڈالو_عقب(ثنائی_ہندسہ);
			برائے (خودکار ثنائی_ہندسہ : پورٹ.srst_قدر)
				rd_srst_قدر_builder.ڈالو_عقب(ثنائی_ہندسہ);
			برائے (خودکار ثنائی_ہندسہ : پورٹ.init_قدر)
				rd_init_قدر_builder.ڈالو_عقب(ثنائی_ہندسہ);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_نقاب = rd_transparency_نقاب_builder.build();
		Const rd_collision_x_نقاب = rd_collision_x_نقاب_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_قدر = rd_arst_قدر_builder.build();
		Const rd_srst_قدر = rd_srst_قدر_builder.build();
		Const rd_init_قدر = rd_init_قدر_builder.build();
		اگر (rd_ports.خالی_خانے()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_قدر = State::S0;
			rd_srst_قدر = State::S0;
			rd_init_قدر = State::S0;
		}
		اگر (rd_ports.خالی_خانے() || wr_ports.خالی_خانے()) {
			rd_transparency_نقاب = State::S0;
			rd_collision_x_نقاب = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_سائز(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_نقاب] = rd_transparency_نقاب;
		cell->parameters[ID::RD_COLLISION_X_نقاب] = rd_collision_x_نقاب;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_قدر] = rd_arst_قدر;
		cell->parameters[ID::RD_SRST_قدر] = rd_srst_قدر;
		cell->parameters[ID::RD_INIT_قدر] = rd_init_قدر;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->سیٹ_پورٹ(ID::RD_CLK, rd_clk);
		cell->سیٹ_پورٹ(ID::RD_EN, rd_en);
		cell->سیٹ_پورٹ(ID::RD_ARST, rd_arst);
		cell->سیٹ_پورٹ(ID::RD_SRST, rd_srst);
		cell->سیٹ_پورٹ(ID::RD_ADDR, rd_addr);
		cell->سیٹ_پورٹ(ID::RD_ڈیٹا, rd_ڈیٹا);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ترجیح_نقاب_builder;
		برائے (خودکار &پورٹ : wr_ports) {
			برائے (خودکار attr: پورٹ.اوصاف)
				اگر (!cell->has_attribute(attr.پہلا))
					cell->اوصاف.داخل_کرو(attr);
			اگر (پورٹ.cell) {
				module->remove(پورٹ.cell);
				پورٹ.cell = خالی_اشاریہ;
			}
			برائے (صحیح sub = 0; sub < (1 << پورٹ.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ڈالو_عقب(State(sub != 0));
				wr_clk_enable_builder.ڈالو_عقب(State(پورٹ.clk_enable));
				wr_clk_polarity_builder.ڈالو_عقب(State(پورٹ.clk_polarity));
				wr_clk.append(پورٹ.clk);
				برائے (خودکار idx : wr_پورٹ_xlat)
					wr_ترجیح_نقاب_builder.ڈالو_عقب(State(منطقی(پورٹ.ترجیح_نقاب[idx])));
				SigSpec addr = پورٹ.sub_addr(sub);
				addr.extend_u0(abits, جھوٹ);
				wr_addr.append(addr);
				log_assert(Get_سائز(addr) == abits);
			}
			wr_en.append(پورٹ.en);
			wr_ڈیٹا.append(پورٹ.ڈیٹا);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ترجیح_نقاب = wr_ترجیح_نقاب_builder.build();
		اگر (wr_ports.خالی_خانے()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ترجیح_نقاب = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_سائز(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ترجیح_نقاب] = wr_ترجیح_نقاب;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->سیٹ_پورٹ(ID::WR_CLK, wr_clk);
		cell->سیٹ_پورٹ(ID::WR_EN, wr_en);
		cell->سیٹ_پورٹ(ID::WR_ADDR, wr_addr);
		cell->سیٹ_پورٹ(ID::WR_ڈیٹا, wr_ڈیٹا);
		برائے (خودکار &init : inits) {
			برائے (خودکار attr: init.اوصاف)
				اگر (!cell->has_attribute(attr.پہلا))
					cell->اوصاف.داخل_کرو(attr);
			اگر (init.cell) {
				module->remove(init.cell);
				init.cell = خالی_اشاریہ;
			}
		}
		cell->parameters[ID::INIT] = get_init_ڈیٹا();
	} ورنہ {
		اگر (cell) {
			module->remove(cell);
			cell = خالی_اشاریہ;
		}
		اگر (!mem) {
			اگر (memid.خالی_خانے())
				memid = PooledName(module->design, module->twines().جوڑو(NEW_ID));
			mem = module->جوڑو_حافظہ(memid);
		}
		mem->چوڑائی = چوڑائی;
		mem->آغاز_انحراف = آغاز_انحراف;
		mem->سائز = سائز;
		mem->اوصاف = اوصاف;
		برائے (خودکار &پورٹ : rd_ports) {
			اگر (!پورٹ.cell)
				پورٹ.cell = module->جوڑو_Cell(NEW_ID, ID($memrd_v2));
			پورٹ.cell->type = ID($memrd_v2);
			پورٹ.cell->اوصاف = پورٹ.اوصاف;
			پورٹ.cell->parameters[ID::MEMID] = memid.str();
			پورٹ.cell->parameters[ID::ABITS] = Get_سائز(پورٹ.addr);
			پورٹ.cell->parameters[ID::چوڑائی_2] = چوڑائی << پورٹ.wide_log2;
			پورٹ.cell->parameters[ID::CLK_ENABLE] = پورٹ.clk_enable;
			پورٹ.cell->parameters[ID::CLK_POLARITY] = پورٹ.clk_polarity;
			پورٹ.cell->parameters[ID::CE_OVER_SRST] = پورٹ.ce_over_srst;
			پورٹ.cell->parameters[ID::ARST_قدر] = پورٹ.arst_قدر;
			پورٹ.cell->parameters[ID::SRST_قدر] = پورٹ.srst_قدر;
			پورٹ.cell->parameters[ID::INIT_قدر] = پورٹ.init_قدر;
			پورٹ.cell->parameters[ID::TRANSPARENCY_نقاب] = پورٹ.transparency_نقاب;
			پورٹ.cell->parameters[ID::COLLISION_X_نقاب] = پورٹ.collision_x_نقاب;
			پورٹ.cell->parameters.erase(ID::TRANSPARENT);
			پورٹ.cell->سیٹ_پورٹ(ID::CLK, پورٹ.clk);
			پورٹ.cell->سیٹ_پورٹ(ID::EN, پورٹ.en);
			پورٹ.cell->سیٹ_پورٹ(ID::ARST, پورٹ.arst);
			پورٹ.cell->سیٹ_پورٹ(ID::SRST, پورٹ.srst);
			پورٹ.cell->سیٹ_پورٹ(ID::ADDR, پورٹ.addr);
			پورٹ.cell->سیٹ_پورٹ(ID::ڈیٹا_2, پورٹ.ڈیٹا);
		}
		صحیح idx = 0;
		برائے (خودکار &پورٹ : wr_ports) {
			اگر (!پورٹ.cell)
				پورٹ.cell = module->جوڑو_Cell(NEW_ID, ID($memwr_v2));
			پورٹ.cell->type = ID($memwr_v2);
			پورٹ.cell->اوصاف = پورٹ.اوصاف;
			اگر (پورٹ.cell->parameters.گنتی(ID::ترجیح))
				پورٹ.cell->parameters.erase(ID::ترجیح);
			پورٹ.cell->parameters[ID::MEMID] = memid.str();
			پورٹ.cell->parameters[ID::ABITS] = Get_سائز(پورٹ.addr);
			پورٹ.cell->parameters[ID::چوڑائی_2] = چوڑائی << پورٹ.wide_log2;
			پورٹ.cell->parameters[ID::CLK_ENABLE] = پورٹ.clk_enable;
			پورٹ.cell->parameters[ID::CLK_POLARITY] = پورٹ.clk_polarity;
			پورٹ.cell->parameters[ID::PORTID] = idx++;
			پورٹ.cell->parameters[ID::ترجیح_نقاب_2] = پورٹ.ترجیح_نقاب;
			پورٹ.cell->سیٹ_پورٹ(ID::CLK, پورٹ.clk);
			پورٹ.cell->سیٹ_پورٹ(ID::EN, پورٹ.en);
			پورٹ.cell->سیٹ_پورٹ(ID::ADDR, پورٹ.addr);
			پورٹ.cell->سیٹ_پورٹ(ID::ڈیٹا_2, پورٹ.ڈیٹا);
		}
		idx = 0;
		برائے (خودکار &init : inits) {
			منطقی v2 = !init.en.is_fully_ones();
			اگر (!init.cell)
				init.cell = module->جوڑو_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			ورنہ {
				اگر (!v2)
					init.cell->unset_پورٹ(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->اوصاف = init.اوصاف;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_سائز(init.addr);
			init.cell->parameters[ID::چوڑائی_2] = چوڑائی;
			init.cell->parameters[ID::الفاظ] = Get_سائز(init.ڈیٹا) / چوڑائی;
			init.cell->parameters[ID::ترجیح] = idx++;
			init.cell->سیٹ_پورٹ(ID::ADDR, init.addr);
			init.cell->سیٹ_پورٹ(ID::ڈیٹا_2, init.ڈیٹا);
			اگر (v2)
				init.cell->سیٹ_پورٹ(ID::EN, init.en);
		}
	}
}

خالی Mem::clear_inits() {
	برائے (خودکار &init : inits)
		init.removed = سچ;
}

خالی Mem::coalesce_inits() {
	// start address -> end address
	std::map<صحیح, صحیح> chunks;
	// Figure out chunk boundaries.
	برائے (خودکار &init : inits) {
		اگر (init.removed)
			جاری;
		منطقی valid = جھوٹ;
		برائے (خودکار ثنائی_ہندسہ : init.en)
			اگر (ثنائی_ہندسہ == State::S1)
				valid = سچ;
		اگر (!valid) {
			init.removed = سچ;
			جاری;
		}
		صحیح addr = init.addr.as_int();
		صحیح addr_e = addr + Get_سائز(init.ڈیٹا) / چوڑائی;
		خودکار it_e = chunks.بالائی_bound(addr_e);
		خودکار it = it_e;
		جب (it != chunks.begin()) {
			--it;
			اگر (it->دوسرا < addr) {
				++it;
				توڑو;
			}
		}
		اگر (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} ورنہ {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			اگر (it->پہلا < addr)
				addr = it->پہلا;
			خودکار it_آخری = it_e;
			it_آخری--;
			اگر (it_آخری->دوسرا > addr_e)
				addr_e = it_آخری->دوسرا;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<صحیح, std::سمتیہ<صحیح>> inits_by_chunk;
	برائے (صحیح i = 0; i < Get_سائز(inits); i++) {
		خودکار &init = inits[i];
		اگر (init.removed)
			جاری;
		خودکار it = chunks.بالائی_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->پہلا].ڈالو_عقب(i);
		صحیح addr = init.addr.as_int();
		صحیح addr_e = addr + Get_سائز(init.ڈیٹا) / چوڑائی;
		log_assert(addr >= it->پہلا && addr_e <= it->دوسرا);
	}
	// Process each chunk.
	برائے (خودکار &it : inits_by_chunk) {
		صحیح caddr = it.پہلا;
		صحیح caddr_e = chunks[caddr];
		خودکار &chunk_inits = it.دوسرا;
		اگر (Get_سائز(chunk_inits) == 1) {
			خودکار &init = inits[chunk_inits[0]];
			اگر (!init.en.is_fully_ones()) {
				برائے (صحیح i = 0; i < Get_سائز(init.ڈیٹا); i++)
					اگر (init.en[i % چوڑائی] != State::S1)
						init.ڈیٹا.سیٹ(i, State::Sx);
				init.en = Const(State::S1, چوڑائی);
			}
			جاری;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * چوڑائی);
		برائے (صحیح idx : chunk_inits) {
			خودکار &init = inits[idx];
			صحیح انحراف = (init.addr.as_int() - caddr) * چوڑائی;
			log_assert(انحراف >= 0);
			log_assert(انحراف + Get_سائز(init.ڈیٹا) <= Get_سائز(cdata));
			برائے (صحیح i = 0; i < Get_سائز(init.ڈیٹا); i++)
				اگر (init.en[i % چوڑائی] == State::S1)
					cdata.سیٹ(i+انحراف, init.ڈیٹا[i]);
			init.removed = سچ;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.ڈیٹا = cdata;
		new_init.en = Const(State::S1, چوڑائی);
		inits.ڈالو_عقب(new_init);
	}
}

Const Mem::get_init_ڈیٹا() ثابت {
	Const init_ڈیٹا(State::Sx, چوڑائی * سائز);
	برائے (خودکار &init : inits) {
		اگر (init.removed)
			جاری;
		صحیح انحراف = (init.addr.as_int() - آغاز_انحراف) * چوڑائی;
		برائے (صحیح i = 0; i < Get_سائز(init.ڈیٹا); i++)
			اگر (0 <= i+انحراف && i+انحراف < Get_سائز(init_ڈیٹا) && init.en[i % چوڑائی] == State::S1)
				init_ڈیٹا.سیٹ(i+انحراف, init.ڈیٹا[i]);
	}
	واپس init_ڈیٹا;
}

خالی Mem::check() {
	صحیح max_wide_log2 = 0;
	برائے (خودکار &پورٹ : rd_ports) {
		اگر (پورٹ.removed)
			جاری;
		log_assert(Get_سائز(پورٹ.clk) == 1);
		log_assert(Get_سائز(پورٹ.en) == 1);
		log_assert(Get_سائز(پورٹ.arst) == 1);
		log_assert(Get_سائز(پورٹ.srst) == 1);
		log_assert(Get_سائز(پورٹ.addr) >= پورٹ.wide_log2);
		log_assert(Get_سائز(پورٹ.ڈیٹا) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_سائز(پورٹ.init_قدر) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_سائز(پورٹ.arst_قدر) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_سائز(پورٹ.srst_قدر) == (چوڑائی << پورٹ.wide_log2));
		اگر (!پورٹ.clk_enable) {
			log_assert(پورٹ.en == State::S1);
			log_assert(پورٹ.arst == State::S0);
			log_assert(پورٹ.srst == State::S0);
		}
		برائے (صحیح j = 0; j < پورٹ.wide_log2; j++) {
			log_assert(پورٹ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٹ.wide_log2);
		log_assert(Get_سائز(پورٹ.transparency_نقاب) == Get_سائز(wr_ports));
		log_assert(Get_سائز(پورٹ.collision_x_نقاب) == Get_سائز(wr_ports));
		برائے (صحیح j = 0; j < Get_سائز(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			اگر ((پورٹ.transparency_نقاب[j] || پورٹ.collision_x_نقاب[j]) && !wport.removed) {
				log_assert(پورٹ.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(پورٹ.clk == wport.clk);
				log_assert(پورٹ.clk_polarity == wport.clk_polarity);
			}
			log_assert(!پورٹ.transparency_نقاب[j] || !پورٹ.collision_x_نقاب[j]);
		}
	}
	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		اگر (پورٹ.removed)
			جاری;
		log_assert(Get_سائز(پورٹ.clk) == 1);
		log_assert(Get_سائز(پورٹ.en) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_سائز(پورٹ.ڈیٹا) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_سائز(پورٹ.addr) >= پورٹ.wide_log2);
		برائے (صحیح j = 0; j < پورٹ.wide_log2; j++) {
			log_assert(پورٹ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٹ.wide_log2);
		log_assert(Get_سائز(پورٹ.ترجیح_نقاب) == Get_سائز(wr_ports));
		برائے (صحیح j = 0; j < Get_سائز(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			اگر (پورٹ.ترجیح_نقاب[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(پورٹ.clk_enable == wport.clk_enable);
				اگر (پورٹ.clk_enable) {
					log_assert(پورٹ.clk == wport.clk);
					log_assert(پورٹ.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	صحیح نقاب = (1 << max_wide_log2) - 1;
	log_assert(!(آغاز_انحراف & نقاب));
	log_assert(!(سائز & نقاب));
	log_assert(چوڑائی != 0);
}

نامخلا {

	ساخت Mem_اشاریہ {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_اشاریہ (Module *module) {
			برائے (خودکار cell: module->cells()) {
				اگر (cell->type.اندر(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
				ورنہ اگر (cell->type.اندر(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
				ورنہ اگر (cell->type.اندر(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
			}
		}
	};

	Mem mem_from_حافظہ(Module *module, RTLIL::حافظہ *mem, ثابت Mem_اشاریہ &اشاریہ) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->چوڑائی, mem->آغاز_انحراف, mem->سائز);
		res.packed = جھوٹ;
		res.mem = mem;
		res.اوصاف = mem->اوصاف;
		std::سمتیہ<منطقی> rd_transparent;
		std::سمتیہ<صحیح> wr_portid;
		اگر (اشاریہ.rd_ports.گنتی(memid)) {
			برائے (خودکار cell : اشاریہ.rd_ports.at(memid)) {
				MemRd mrd;
				منطقی is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.اوصاف = cell->اوصاف;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_پورٹ(ID::CLK);
				mrd.en = cell->get_پورٹ(ID::EN);
				mrd.addr = cell->get_پورٹ(ID::ADDR);
				mrd.ڈیٹا = cell->get_پورٹ(ID::ڈیٹا_2);
				mrd.wide_log2 = ceil_log2(Get_سائز(mrd.ڈیٹا) / mem->چوڑائی);
				منطقی transparent = جھوٹ;
				اگر (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = جھوٹ;
					mrd.arst_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.srst_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.init_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					اگر (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = جھوٹ;
						اگر (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} ورنہ {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_قدر = cell->parameters.at(ID::ARST_قدر);
					mrd.srst_قدر = cell->parameters.at(ID::SRST_قدر);
					mrd.init_قدر = cell->parameters.at(ID::INIT_قدر);
					mrd.arst = cell->get_پورٹ(ID::ARST);
					mrd.srst = cell->get_پورٹ(ID::SRST);
				}
				res.rd_ports.ڈالو_عقب(mrd);
				rd_transparent.ڈالو_عقب(transparent);
			}
		}
		اگر (اشاریہ.wr_ports.گنتی(memid)) {
			std::سمتیہ<std::pair<صحیح, MemWr>> ports;
			برائے (خودکار cell : اشاریہ.wr_ports.at(memid)) {
				MemWr mwr;
				منطقی is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.اوصاف = cell->اوصاف;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_پورٹ(ID::CLK);
				mwr.en = cell->get_پورٹ(ID::EN);
				mwr.addr = cell->get_پورٹ(ID::ADDR);
				mwr.ڈیٹا = cell->get_پورٹ(ID::ڈیٹا_2);
				mwr.wide_log2 = ceil_log2(Get_سائز(mwr.ڈیٹا) / mem->چوڑائی);
				ports.ڈالو_عقب(std::make_pair(cell->parameters.at(is_compat ? ID::ترجیح : ID::PORTID).as_int(), mwr));
			}
			std::ترتیب_دو(ports.begin(), ports.انتہا(), [](ثابت std::pair<صحیح, MemWr> &a, ثابت std::pair<صحیح, MemWr> &b) { واپس a.پہلا < b.پہلا; });
			برائے (خودکار &it : ports) {
				res.wr_ports.ڈالو_عقب(it.دوسرا);
				wr_portid.ڈالو_عقب(it.پہلا);
			}
			برائے (صحیح i = 0; i < Get_سائز(res.wr_ports); i++) {
				خودکار &پورٹ = res.wr_ports[i];
				منطقی is_compat = پورٹ.cell->type == ID($memwr);
				اگر (is_compat) {
					پورٹ.ترجیح_نقاب.resize(Get_سائز(res.wr_ports));
					برائے (صحیح j = 0; j < i; j++) {
						خودکار &oport = res.wr_ports[j];
						اگر (پورٹ.clk_enable != oport.clk_enable)
							جاری;
						اگر (پورٹ.clk_enable && پورٹ.clk != oport.clk)
							جاری;
						اگر (پورٹ.clk_enable && پورٹ.clk_polarity != oport.clk_polarity)
							جاری;
						پورٹ.ترجیح_نقاب[j] = سچ;
					}
				} ورنہ {
					Const orig_prio_نقاب = پورٹ.cell->parameters.at(ID::ترجیح_نقاب_2);
					برائے (صحیح orig_portid : wr_portid) {
						منطقی has_prio = orig_portid < Get_سائز(orig_prio_نقاب) && orig_prio_نقاب[orig_portid] == State::S1;
						پورٹ.ترجیح_نقاب.ڈالو_عقب(has_prio);
					}
				}
			}
		}
		اگر (اشاریہ.inits.گنتی(memid)) {
			std::سمتیہ<std::pair<صحیح, MemInit>> inits;
			برائے (خودکار cell : اشاریہ.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.اوصاف = cell->اوصاف;
				خودکار addr = cell->get_پورٹ(ID::ADDR);
				خودکار ڈیٹا = cell->get_پورٹ(ID::ڈیٹا_2);
				اگر (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				اگر (!ڈیٹا.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(ڈیٹا), cell);
				init.addr = addr.as_const();
				init.ڈیٹا = ڈیٹا.as_const();
				اگر (cell->type == ID($meminit_v2)) {
					خودکار en = cell->get_پورٹ(ID::EN);
					اگر (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} ورنہ {
					init.en = RTLIL::Const(State::S1, mem->چوڑائی);
				}
				inits.ڈالو_عقب(std::make_pair(cell->parameters.at(ID::ترجیح).as_int(), init));
			}
			std::ترتیب_دو(inits.begin(), inits.انتہا(), [](ثابت std::pair<صحیح, MemInit> &a, ثابت std::pair<صحیح, MemInit> &b) { واپس a.پہلا < b.پہلا; });
			برائے (خودکار &it : inits)
				res.inits.ڈالو_عقب(it.دوسرا);
		}
		برائے (صحیح i = 0; i < Get_سائز(res.rd_ports); i++) {
			خودکار &پورٹ = res.rd_ports[i];
			منطقی is_compat = پورٹ.cell->type == ID($memrd);
			اگر (is_compat) {
				پورٹ.transparency_نقاب.resize(Get_سائز(res.wr_ports));
				پورٹ.collision_x_نقاب.resize(Get_سائز(res.wr_ports));
				اگر (!rd_transparent[i])
					جاری;
				اگر (!پورٹ.clk_enable)
					جاری;
				برائے (صحیح j = 0; j < Get_سائز(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					اگر (!wport.clk_enable)
						جاری;
					اگر (پورٹ.clk != wport.clk)
						جاری;
					اگر (پورٹ.clk_polarity != wport.clk_polarity)
						جاری;
					پورٹ.transparency_نقاب[j] = سچ;
				}
			} ورنہ {
				Const orig_trans_نقاب = پورٹ.cell->parameters.at(ID::TRANSPARENCY_نقاب);
				Const orig_cx_نقاب = پورٹ.cell->parameters.at(ID::COLLISION_X_نقاب);
				برائے (صحیح orig_portid : wr_portid) {
					پورٹ.transparency_نقاب.ڈالو_عقب(orig_portid < Get_سائز(orig_trans_نقاب) && orig_trans_نقاب[orig_portid] == State::S1);
					پورٹ.collision_x_نقاب.ڈالو_عقب(orig_portid < Get_سائز(orig_cx_نقاب) && orig_cx_نقاب[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		واپس res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().جوڑو(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::چوڑائی_2).as_int(),
			cell->parameters.at(ID::انحراف_2).as_int(),
			cell->parameters.at(ID::سائز_2).as_int()
		);
		منطقی is_compat = cell->type == ID($mem);
		صحیح abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = سچ;
		res.cell = cell;
		res.اوصاف = cell->اوصاف;
		Const &init = cell->parameters.at(ID::INIT);
		اگر (!init.is_fully_undef()) {
			صحیح pos = 0;
			جب (pos < res.سائز) {
				Const لفظ = init.نکالو_2(pos * res.چوڑائی, res.چوڑائی, State::Sx);
				اگر (لفظ.is_fully_undef()) {
					pos++;
				} ورنہ {
					صحیح epos;
					برائے (epos = pos; epos < res.سائز; epos++) {
						Const eword = init.نکالو_2(epos * res.چوڑائی, res.چوڑائی, State::Sx);
						اگر (eword.is_fully_undef())
							توڑو;
					}
					MemInit minit;
					minit.addr = res.آغاز_انحراف + pos;
					minit.ڈیٹا = init.نکالو_2(pos * res.چوڑائی, (epos - pos) * res.چوڑائی, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.چوڑائی);
					res.inits.ڈالو_عقب(minit);
					pos = epos;
				}
			}
		}
		صحیح n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		صحیح n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		برائے (صحیح i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			جب (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).نکالو_2(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).نکالو_2(i, 1).as_bool();
			mrd.clk = cell->get_پورٹ(ID::RD_CLK).نکالو_2(i, 1);
			mrd.en = cell->get_پورٹ(ID::RD_EN).نکالو_2(i, 1);
			mrd.addr = cell->get_پورٹ(ID::RD_ADDR).نکالو_2(i * abits, abits);
			mrd.ڈیٹا = cell->get_پورٹ(ID::RD_ڈیٹا).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			اگر (is_compat) {
				mrd.ce_over_srst = جھوٹ;
				mrd.arst_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.srst_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.init_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} ورنہ {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).نکالو_2(i, 1).as_bool();
				mrd.arst_قدر = cell->parameters.at(ID::RD_ARST_قدر).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.srst_قدر = cell->parameters.at(ID::RD_SRST_قدر).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.init_قدر = cell->parameters.at(ID::RD_INIT_قدر).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.arst = cell->get_پورٹ(ID::RD_ARST).نکالو_2(i, 1);
				mrd.srst = cell->get_پورٹ(ID::RD_SRST).نکالو_2(i, 1);
			}
			اگر (!is_compat) {
				Const transparency_نقاب = cell->parameters.at(ID::RD_TRANSPARENCY_نقاب).نکالو_2(i * n_wr_ports, n_wr_ports);
				Const collision_x_نقاب = cell->parameters.at(ID::RD_COLLISION_X_نقاب).نکالو_2(i * n_wr_ports, n_wr_ports);
				برائے (صحیح j = 0; j < n_wr_ports; j++)
					اگر (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_نقاب.ڈالو_عقب(transparency_نقاب[j] == State::S1);
						mrd.collision_x_نقاب.ڈالو_عقب(collision_x_نقاب[j] == State::S1);
					}
			}
			res.rd_ports.ڈالو_عقب(mrd);
		}
		برائے (صحیح i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			جب (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).نکالو_2(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).نکالو_2(i, 1).as_bool();
			mwr.clk = cell->get_پورٹ(ID::WR_CLK).نکالو_2(i, 1);
			mwr.en = cell->get_پورٹ(ID::WR_EN).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			mwr.addr = cell->get_پورٹ(ID::WR_ADDR).نکالو_2(i * abits, abits);
			mwr.ڈیٹا = cell->get_پورٹ(ID::WR_ڈیٹا).نکالو_2(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			اگر (!is_compat) {
				Const ترجیح_نقاب = cell->parameters.at(ID::WR_ترجیح_نقاب).نکالو_2(i * n_wr_ports, n_wr_ports);
				برائے (صحیح j = 0; j < n_wr_ports; j++)
					اگر (wr_wide_continuation[j] != State::S1)
						mwr.ترجیح_نقاب.ڈالو_عقب(ترجیح_نقاب[j] == State::S1);
			}
			res.wr_ports.ڈالو_عقب(mwr);
		}
		اگر (is_compat) {
			برائے (صحیح i = 0; i < Get_سائز(res.wr_ports); i++) {
				خودکار &پورٹ = res.wr_ports[i];
				پورٹ.ترجیح_نقاب.resize(Get_سائز(res.wr_ports));
				برائے (صحیح j = 0; j < i; j++) {
					خودکار &oport = res.wr_ports[j];
					اگر (پورٹ.clk_enable != oport.clk_enable)
						جاری;
					اگر (پورٹ.clk_enable && پورٹ.clk != oport.clk)
						جاری;
					اگر (پورٹ.clk_enable && پورٹ.clk_polarity != oport.clk_polarity)
						جاری;
					پورٹ.ترجیح_نقاب[j] = سچ;
				}
			}
			برائے (صحیح i = 0; i < Get_سائز(res.rd_ports); i++) {
				خودکار &پورٹ = res.rd_ports[i];
				پورٹ.transparency_نقاب.resize(Get_سائز(res.wr_ports));
				پورٹ.collision_x_نقاب.resize(Get_سائز(res.wr_ports));
				اگر (!cell->parameters.at(ID::RD_TRANSPARENT).نکالو_2(i, 1).as_bool())
					جاری;
				اگر (!پورٹ.clk_enable)
					جاری;
				برائے (صحیح j = 0; j < Get_سائز(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					اگر (!wport.clk_enable)
						جاری;
					اگر (پورٹ.clk != wport.clk)
						جاری;
					اگر (پورٹ.clk_polarity != wport.clk_polarity)
						جاری;
					پورٹ.transparency_نقاب[j] = سچ;
				}
			}
		}
		res.check();
		واپس res;
	}

}

std::سمتیہ<Mem> Mem::get_all_memories(Module *module) {
	std::سمتیہ<Mem> res;
	Mem_اشاریہ اشاریہ(module);
	برائے (خودکار it: module->memories) {
		res.ڈالو_عقب(mem_from_حافظہ(module, it.دوسرا, اشاریہ));
	}
	برائے (خودکار cell: module->cells()) {
		اگر (cell->type.اندر(ID($mem), ID($mem_v2)))
			res.ڈالو_عقب(mem_from_cell(cell));
	}
	واپس res;
}

std::سمتیہ<Mem> Mem::get_selected_memories(Module *module) {
	std::سمتیہ<Mem> res;
	Mem_اشاریہ اشاریہ(module);
	برائے (خودکار it: module->memories) {
		اگر (module->design->selected(module, it.دوسرا))
			res.ڈالو_عقب(mem_from_حافظہ(module, it.دوسرا, اشاریہ));
	}
	برائے (خودکار cell: module->selected_cells()) {
		اگر (cell->type.اندر(ID($mem), ID($mem_v2)))
			res.ڈالو_عقب(mem_from_cell(cell));
	}
	واپس res;
}

Cell *Mem::نکالو_rdff(صحیح idx, FfInitVals *initvals) {
	MemRd &پورٹ = rd_ports[idx];

	اگر (!پورٹ.clk_enable)
		واپس خالی_اشاریہ;

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
	منطقی trans_use_addr = سچ;
	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++)
		اگر (!پورٹ.transparency_نقاب[i] && !wr_ports[i].removed)
			trans_use_addr = جھوٹ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	اگر (Get_سائز(wr_ports) == 0)
		trans_use_addr = جھوٹ;

	اگر (پورٹ.en != State::S1 || پورٹ.srst != State::S0 || پورٹ.arst != State::S0 || !پورٹ.init_قدر.is_fully_undef())
		trans_use_addr = جھوٹ;

	اگر (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		صحیح چوڑائی = 0;
		برائے (صحیح i = 0; i < Get_سائز(پورٹ.addr); i++)
			اگر (پورٹ.addr[i].wire)
				چوڑائی++;

		اگر (چوڑائی)
		{
			SigSpec sig_q = module->جوڑو_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), چوڑائی);
			SigSpec sig_d;

			صحیح pos = 0;
			برائے (صحیح i = 0; i < Get_سائز(پورٹ.addr); i++)
				اگر (پورٹ.addr[i].wire) {
					sig_d.append(پورٹ.addr[i]);
					پورٹ.addr[i] = sig_q[pos++];
				}

			c = module->جوڑو_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), پورٹ.clk, sig_d, sig_q, پورٹ.clk_polarity);
		} ورنہ {
			c = خالی_اشاریہ;
		}
	}
	ورنہ
	{
		log_assert(پورٹ.arst == State::S0 || پورٹ.srst == State::S0);

		SigSpec async_d = module->جوڑو_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_سائز(پورٹ.ڈیٹا));
		SigSpec sig_d = async_d;

		برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
			خودکار &wport = wr_ports[i];
			اگر (wport.removed)
				جاری;
			اگر (پورٹ.transparency_نقاب[i] || پورٹ.collision_x_نقاب[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == پورٹ.clk);
				log_assert(wport.clk_enable == پورٹ.clk_enable);
				صحیح min_wide_log2 = std::min(پورٹ.wide_log2, wport.wide_log2);
				صحیح max_wide_log2 = std::max(پورٹ.wide_log2, wport.wide_log2);
				منطقی wide_write = wport.wide_log2 > پورٹ.wide_log2;
				برائے (صحیح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = پورٹ.addr;
					SigSpec waddr = wport.addr;
					اگر (wide_write)
						waddr = wport.sub_addr(sub);
					ورنہ
						raddr = پورٹ.sub_addr(sub);
					SigSpec addr_eq;
					اگر (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					صحیح pos = 0;
					صحیح ewidth = چوڑائی << min_wide_log2;
					صحیح wsub = wide_write ? sub : 0;
					صحیح rsub = wide_write ? 0 : sub;
					جب (pos < ewidth) {
						صحیح epos = pos;
						جب (epos < ewidth && wport.en[epos + wsub * چوڑائی] == wport.en[pos + wsub * چوڑائی])
							epos++;
						SigSpec cur = sig_d.نکالو_2(pos + rsub * چوڑائی, epos-pos);
						SigSpec other = پورٹ.transparency_نقاب[i] ? wport.ڈیٹا.نکالو_2(pos + wsub * چوڑائی, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						اگر (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * چوڑائی], addr_eq);
						ورنہ
							cond = wport.en[pos + wsub * چوڑائی];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * چوڑائی, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().جوڑو(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_ڈیٹا ff(module, initvals, name);
		ff.چوڑائی = Get_سائز(پورٹ.ڈیٹا);
		ff.has_clk = سچ;
		ff.sig_clk = پورٹ.clk;
		ff.pol_clk = پورٹ.clk_polarity;
		اگر (پورٹ.en != State::S1) {
			ff.has_ce = سچ;
			ff.pol_ce = سچ;
			ff.sig_ce = پورٹ.en;
		}
		اگر (پورٹ.arst != State::S0) {
			ff.has_arst = سچ;
			ff.pol_arst = سچ;
			ff.sig_arst = پورٹ.arst;
			ff.val_arst = پورٹ.arst_قدر;
		}
		اگر (پورٹ.srst != State::S0) {
			ff.has_srst = سچ;
			ff.pol_srst = سچ;
			ff.sig_srst = پورٹ.srst;
			ff.val_srst = پورٹ.srst_قدر;
			ff.ce_over_srst = ff.has_ce && پورٹ.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = پورٹ.ڈیٹا;
		ff.val_init = پورٹ.init_قدر;
		پورٹ.ڈیٹا = async_d;
		c = ff.emit();
	}

	اگر (c)
		لوگارتھم("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	پورٹ.en = State::S1;
	پورٹ.clk = State::S0;
	پورٹ.arst = State::S0;
	پورٹ.srst = State::S0;
	پورٹ.clk_enable = جھوٹ;
	پورٹ.clk_polarity = سچ;
	پورٹ.ce_over_srst = جھوٹ;
	پورٹ.arst_قدر = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));
	پورٹ.srst_قدر = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));
	پورٹ.init_قدر = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));

	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
		پورٹ.transparency_نقاب[i] = جھوٹ;
		پورٹ.collision_x_نقاب[i] = جھوٹ;
	}

	واپس c;
}

خالی Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::سمتیہ<MemRd> new_rd_ports;
	std::سمتیہ<MemWr> new_wr_ports;
	std::سمتیہ<std::pair<صحیح, صحیح>> new_rd_map;
	std::سمتیہ<std::pair<صحیح, صحیح>> new_wr_map;
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++) {
		خودکار &پورٹ = rd_ports[i];
		برائے (صحیح sub = 0; sub < (1 << پورٹ.wide_log2); sub++) {
			new_rd_map.ڈالو_عقب(std::make_pair(i, sub));
		}
	}
	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		برائے (صحیح sub = 0; sub < (1 << پورٹ.wide_log2); sub++) {
			new_wr_map.ڈالو_عقب(std::make_pair(i, sub));
		}
	}
	برائے (خودکار &it : new_rd_map) {
		MemRd &orig = rd_ports[it.پہلا];
		MemRd پورٹ = orig;
		اگر (it.دوسرا != 0)
			پورٹ.cell = خالی_اشاریہ;
		اگر (پورٹ.wide_log2) {
			پورٹ.ڈیٹا = پورٹ.ڈیٹا.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.init_قدر = پورٹ.init_قدر.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.arst_قدر = پورٹ.arst_قدر.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.srst_قدر = پورٹ.srst_قدر.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.addr = پورٹ.sub_addr(it.دوسرا);
			پورٹ.wide_log2 = 0;
		}
		پورٹ.transparency_نقاب.clear();
		پورٹ.collision_x_نقاب.clear();
		برائے (خودکار &it2 : new_wr_map)
			پورٹ.transparency_نقاب.ڈالو_عقب(orig.transparency_نقاب[it2.پہلا]);
		برائے (خودکار &it2 : new_wr_map)
			پورٹ.collision_x_نقاب.ڈالو_عقب(orig.collision_x_نقاب[it2.پہلا]);
		new_rd_ports.ڈالو_عقب(پورٹ);
	}
	برائے (خودکار &it : new_wr_map) {
		MemWr &orig = wr_ports[it.پہلا];
		MemWr پورٹ = orig;
		اگر (it.دوسرا != 0)
			پورٹ.cell = خالی_اشاریہ;
		اگر (پورٹ.wide_log2) {
			پورٹ.ڈیٹا = پورٹ.ڈیٹا.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.en = پورٹ.en.نکالو_2(it.دوسرا * چوڑائی, چوڑائی);
			پورٹ.addr = پورٹ.sub_addr(it.دوسرا);
			پورٹ.wide_log2 = 0;
		}
		پورٹ.ترجیح_نقاب.clear();
		برائے (خودکار &it2 : new_wr_map)
			پورٹ.ترجیح_نقاب.ڈالو_عقب(orig.ترجیح_نقاب[it2.پہلا]);
		new_wr_ports.ڈالو_عقب(پورٹ);
	}
	std::تبادلہ(rd_ports, new_rd_ports);
	std::تبادلہ(wr_ports, new_wr_ports);
}

خالی Mem::emulate_ترجیح(صحیح idx1, صحیح idx2, FfInitVals *initvals)
{
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	اگر (!port2.ترجیح_نقاب[idx1])
		واپس;
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		اگر (rport.removed)
			جاری;
		اگر (rport.transparency_نقاب[idx1] && !(rport.transparency_نقاب[idx2] || rport.collision_x_نقاب[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	صحیح min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	صحیح max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	منطقی wide1 = port1.wide_log2 > port2.wide_log2;
	برائے (صحیح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		اگر (wide1)
			addr1 = port1.sub_addr(sub);
		ورنہ
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		صحیح ewidth = چوڑائی << min_wide_log2;
		صحیح sub1 = wide1 ? sub : 0;
		صحیح sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ>, Sig_ثنائی_ہندسہ> cache;
		برائے (صحیح pos = 0; pos < ewidth; pos++) {
			Sig_ثنائی_ہندسہ &en1 = port1.en[pos + sub1 * چوڑائی];
			Sig_ثنائی_ہندسہ &en2 = port2.en[pos + sub2 * چوڑائی];
			std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ> کلید(en1, en2);
			اگر (cache.گنتی(کلید)) {
				en1 = cache[کلید];
			} ورنہ {
				Sig_ثنائی_ہندسہ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_ثنائی_ہندسہ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[کلید] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ترجیح_نقاب[idx1] = جھوٹ;
}

خالی Mem::emulate_transparency(صحیح widx, صحیح ridx, FfInitVals *initvals) {
	خودکار &wport = wr_ports[widx];
	خودکار &rport = rd_ports[ridx];
	log_assert(rport.transparency_نقاب[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	برائے (صحیح i = Get_سائز(wr_ports) - 1; i > widx; i--) {
		اگر (wr_ports[i].removed)
			جاری;
		اگر (rport.transparency_نقاب[i] && wr_ports[i].ترجیح_نقاب[widx])
			emulate_transparency(i, ridx, initvals);
	}
	صحیح min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	صحیح max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	منطقی wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->جوڑو_Wire(NEW_ID, Get_سائز(wport.ڈیٹا));
	module->جوڑو_Dffe(NEW_ID, rport.clk, rport.en, wport.ڈیٹا, wdata_q, rport.clk_polarity, سچ);
	برائے (صحیح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		برائے (صحیح j = min_wide_log2; j < max_wide_log2; j++)
			اگر (wide_write)
				waddr = wport.sub_addr(sub);
			ورنہ
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		اگر (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		صحیح pos = 0;
		صحیح ewidth = چوڑائی << min_wide_log2;
		صحیح wsub = wide_write ? sub : 0;
		صحیح rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->جوڑو_Wire(NEW_ID, ewidth);
		جب (pos < ewidth) {
			صحیح epos = pos;
			جب (epos < ewidth && wport.en[epos + wsub * چوڑائی] == wport.en[pos + wsub * چوڑائی])
				epos++;
			SigSpec cond;
			اگر (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * چوڑائی], addr_eq);
			ورنہ
				cond = wport.en[pos + wsub * چوڑائی];
			SigSpec cond_q = module->جوڑو_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_ڈیٹا ff(module, initvals, NEW_ID);
			ff.چوڑائی = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = سچ;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			اگر (rport.en != State::S1) {
				ff.has_ce = سچ;
				ff.sig_ce = rport.en;
				ff.pol_ce = سچ;
			}
			اگر (rport.arst != State::S0) {
				ff.has_arst = سچ;
				ff.sig_arst = rport.arst;
				ff.pol_arst = سچ;
				ff.val_arst = State::S0;
			}
			اگر (rport.srst != State::S0) {
				ff.has_srst = سچ;
				ff.sig_srst = rport.srst;
				ff.pol_srst = سچ;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			اگر (!rport.init_قدر.is_fully_undef())
				ff.val_init = State::S0;
			ورنہ
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.نکالو_2(pos, epos-pos);
			SigSpec other = wdata_q.نکالو_2(pos + wsub * چوڑائی, epos-pos);
			SigSpec dest = rport.ڈیٹا.نکالو_2(pos + rsub * چوڑائی, epos-pos);
			module->جوڑو_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.ڈیٹا.replace(rsub * چوڑائی, rdata_a);
	}
	rport.transparency_نقاب[widx] = جھوٹ;
	rport.collision_x_نقاب[widx] = سچ;
}

خالی Mem::prepare_wr_ملاؤ(صحیح idx1, صحیح idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	برائے (صحیح i = 0; i < idx1; i++)
		اگر (port2.ترجیح_نقاب[i])
			port1.ترجیح_نقاب[i] = سچ;
	// If port 2 has priority over a port after port 1, emulate it.
	برائے (صحیح i = idx1 + 1; i < idx2; i++)
		اگر (port2.ترجیح_نقاب[i] && !wr_ports[i].removed)
			emulate_ترجیح(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	برائے (صحیح i = idx2 + 1; i < Get_سائز(wr_ports); i++) {
		خودکار &oport = wr_ports[i];
		اگر (oport.ترجیح_نقاب[idx2])
			oport.ترجیح_نقاب[idx1] = سچ;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		اگر (rport.removed)
			جاری;
		// If collision already undefined with both ports, it's fine.
		اگر (rport.collision_x_نقاب[idx1] && rport.collision_x_نقاب[idx2])
			جاری;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		اگر (rport.collision_x_نقاب[idx1]) {
			rport.collision_x_نقاب[idx1] = جھوٹ;
			rport.transparency_نقاب[idx1] = rport.transparency_نقاب[idx2];
			جاری;
		}
		اگر (rport.collision_x_نقاب[idx2]) {
			rport.collision_x_نقاب[idx2] = جھوٹ;
			rport.transparency_نقاب[idx2] = rport.transparency_نقاب[idx1];
			جاری;
		}
		// If transparent with both ports, also fine.
		اگر (rport.transparency_نقاب[idx1] && rport.transparency_نقاب[idx2])
			جاری;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		اگر (rport.transparency_نقاب[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_نقاب[idx1] = جھوٹ;
			جاری;
		}
		اگر (rport.transparency_نقاب[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_نقاب[idx2] = جھوٹ;
			جاری;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

خالی Mem::prepare_rd_ملاؤ(صحیح idx1, صحیح idx2, FfInitVals *initvals) {
	خودکار &port1 = rd_ports[idx1];
	خودکار &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	برائے (صحیح i = 0; i < Get_سائز(wr_ports); i++) {
		اگر (wr_ports[i].removed)
			جاری;
		// Both ports undefined, OK.
		اگر (port1.collision_x_نقاب[i] && port2.collision_x_نقاب[i])
			جاری;
		// Only one port undefined — change its behavior
		// to align with the other port.
		اگر (port1.collision_x_نقاب[i]) {
			port1.collision_x_نقاب[i] = جھوٹ;
			port1.transparency_نقاب[i] = port2.transparency_نقاب[i];
			جاری;
		}
		اگر (port2.collision_x_نقاب[i]) {
			port2.collision_x_نقاب[i] = جھوٹ;
			port2.transparency_نقاب[i] = port1.transparency_نقاب[i];
			جاری;
		}
		// Both ports transparent, OK.
		اگر (port1.transparency_نقاب[i] && port2.transparency_نقاب[i])
			جاری;
		// Only one port transparent — emulate transparency
		// on the other.
		اگر (port1.transparency_نقاب[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_نقاب[i] = جھوٹ;
			جاری;
		}
		اگر (port2.transparency_نقاب[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_نقاب[i] = جھوٹ;
			جاری;
		}
		// No ports transparent, OK.
	}

}

خالی Mem::widen_prep(صحیح wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	صحیح نقاب = ((1 << wide_log2) - 1);
	صحیح delta = آغاز_انحراف & نقاب;
	آغاز_انحراف -= delta;
	سائز += delta;
	اگر (سائز & نقاب) {
		سائز |= نقاب;
		سائز++;
	}
}

خالی Mem::widen_wr_پورٹ(صحیح idx, صحیح wide_log2) {
	widen_prep(wide_log2);
	خودکار &پورٹ = wr_ports[idx];
	log_assert(پورٹ.wide_log2 <= wide_log2);
	اگر (پورٹ.wide_log2 < wide_log2) {
		SigSpec new_ڈیٹا, new_en;
		SigSpec addr_lo = پورٹ.addr.نکالو_2(0, wide_log2);
		برائے (صحیح sub = 0; sub < (1 << wide_log2); sub += (1 << پورٹ.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			اگر (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_ڈیٹا.append(پورٹ.ڈیٹا);
				new_en.append(پورٹ.en);
			} ورنہ اگر (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_ڈیٹا.append(Const(State::Sx, Get_سائز(پورٹ.ڈیٹا)));
				new_en.append(Const(State::S0, Get_سائز(پورٹ.ڈیٹا)));
			} ورنہ {
				// May or may not write to this subword.
				new_ڈیٹا.append(پورٹ.ڈیٹا);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_سائز(پورٹ.ڈیٹا)), پورٹ.en, addr_eq);
				new_en.append(en);
			}
		}
		پورٹ.addr.replace(پورٹ.wide_log2, Const(State::S0, wide_log2 - پورٹ.wide_log2));
		پورٹ.ڈیٹا = new_ڈیٹا;
		پورٹ.en = new_en;
		پورٹ.wide_log2 = wide_log2;
	}
}

خالی Mem::emulate_rden(صحیح idx, FfInitVals *initvals) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
	Wire *پچھلا_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
	Wire *sel = module->جوڑو_Wire(NEW_ID);
	Ff_ڈیٹا ff_sel(module, initvals, NEW_ID);
	Ff_ڈیٹا ff_ڈیٹا(module, initvals, NEW_ID);
	ff_sel.چوڑائی = 1;
	ff_sel.has_clk = سچ;
	ff_sel.sig_clk = پورٹ.clk;
	ff_sel.pol_clk = پورٹ.clk_polarity;
	ff_sel.sig_d = پورٹ.en;
	ff_sel.sig_q = sel;
	ff_ڈیٹا.چوڑائی = Get_سائز(پورٹ.ڈیٹا);
	ff_ڈیٹا.has_clk = سچ;
	ff_ڈیٹا.sig_clk = پورٹ.clk;
	ff_ڈیٹا.pol_clk = پورٹ.clk_polarity;
	ff_ڈیٹا.sig_d = پورٹ.ڈیٹا;
	ff_ڈیٹا.sig_q = پچھلا_ڈیٹا;
	اگر (!پورٹ.init_قدر.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_ڈیٹا.val_init = پورٹ.init_قدر;
		پورٹ.init_قدر = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));
	} ورنہ {
		ff_sel.val_init = State::Sx;
		ff_ڈیٹا.val_init = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));
	}
	اگر (پورٹ.arst != State::S0) {
		ff_sel.has_arst = سچ;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = پورٹ.arst;
		ff_sel.pol_arst = سچ;
		ff_ڈیٹا.has_arst = سچ;
		ff_ڈیٹا.val_arst = پورٹ.arst_قدر;
		ff_ڈیٹا.sig_arst = پورٹ.arst;
		ff_ڈیٹا.pol_arst = سچ;
		پورٹ.arst = State::S0;
	}
	اگر (پورٹ.srst != State::S0) {
		log_assert(!پورٹ.ce_over_srst);
		ff_sel.has_srst = سچ;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = پورٹ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.ce_over_srst = جھوٹ;
		ff_ڈیٹا.has_srst = سچ;
		ff_ڈیٹا.val_srst = پورٹ.srst_قدر;
		ff_ڈیٹا.sig_srst = پورٹ.srst;
		ff_ڈیٹا.pol_srst = سچ;
		ff_ڈیٹا.ce_over_srst = جھوٹ;
		پورٹ.srst = State::S0;
	}
	ff_sel.emit();
	ff_ڈیٹا.emit();
	module->جوڑو_Mux(NEW_ID, پچھلا_ڈیٹا, new_ڈیٹا, sel, پورٹ.ڈیٹا);
	پورٹ.ڈیٹا = new_ڈیٹا;
	پورٹ.en = State::S1;
}

خالی Mem::emulate_reset(صحیح idx, منطقی emu_init, منطقی emu_arst, منطقی emu_srst, FfInitVals *initvals) {
	خودکار &پورٹ = rd_ports[idx];
	اگر (emu_init && !پورٹ.init_قدر.is_fully_undef()) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_ڈیٹا ff_sel(module, initvals, NEW_ID);
		Wire *new_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		اگر (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		اگر (پورٹ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٹ.arst;
			ff_sel.pol_arst = سچ;
			اگر (emu_arst && پورٹ.arst_قدر == پورٹ.init_قدر) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				پورٹ.arst = State::S0;
			} ورنہ {
				ff_sel.val_arst = State::S1;
			}
		}
		اگر (پورٹ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٹ.srst;
			ff_sel.pol_srst = سچ;
			اگر (emu_srst && پورٹ.srst_قدر == پورٹ.init_قدر) {
				ff_sel.val_srst = State::S0;
				پورٹ.srst = State::S0;
			} ورنہ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.init_قدر, new_ڈیٹا, sel, پورٹ.ڈیٹا);
		پورٹ.ڈیٹا = new_ڈیٹا;
		پورٹ.init_قدر = Const(State::Sx, Get_سائز(پورٹ.ڈیٹا));
	}
	اگر (emu_arst && پورٹ.arst != State::S0) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_ڈیٹا ff_sel(module, initvals, NEW_ID);
		Wire *new_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		اگر (پورٹ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ورنہ
			ff_sel.val_init = State::S1;
		اگر (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		ff_sel.has_arst = سچ;
		ff_sel.sig_arst = پورٹ.arst;
		ff_sel.pol_arst = سچ;
		ff_sel.val_arst = State::S0;
		اگر (پورٹ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٹ.srst;
			ff_sel.pol_srst = سچ;
			اگر (emu_srst && پورٹ.srst_قدر == پورٹ.arst_قدر) {
				ff_sel.val_srst = State::S0;
				پورٹ.srst = State::S0;
			} ورنہ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.arst_قدر, new_ڈیٹا, sel, پورٹ.ڈیٹا);
		پورٹ.ڈیٹا = new_ڈیٹا;
		پورٹ.arst = State::S0;
	}
	اگر (emu_srst && پورٹ.srst != State::S0) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_ڈیٹا ff_sel(module, initvals, NEW_ID);
		Wire *new_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		اگر (پورٹ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ورنہ
			ff_sel.val_init = State::S1;
		اگر (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		ff_sel.has_srst = سچ;
		ff_sel.sig_srst = پورٹ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.val_srst = State::S0;
		اگر (پورٹ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٹ.arst;
			ff_sel.pol_arst = سچ;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.srst_قدر, new_ڈیٹا, sel, پورٹ.ڈیٹا);
		پورٹ.ڈیٹا = new_ڈیٹا;
		پورٹ.srst = State::S0;
	}
}

خالی Mem::emulate_rd_ce_over_srst(صحیح idx) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	اگر (پورٹ.en == State::S1 || پورٹ.srst == State::S0 || !پورٹ.ce_over_srst) {
		پورٹ.ce_over_srst = جھوٹ;
		واپس;
	}
	پورٹ.ce_over_srst = جھوٹ;
	پورٹ.srst = module->And(NEW_ID, پورٹ.en, پورٹ.srst);
}

خالی Mem::emulate_rd_srst_over_ce(صحیح idx) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	اگر (پورٹ.en == State::S1 || پورٹ.srst == State::S0 || پورٹ.ce_over_srst) {
		پورٹ.ce_over_srst = سچ;
		واپس;
	}
	پورٹ.ce_over_srst = سچ;
	پورٹ.en = module->Or(NEW_ID, پورٹ.en, پورٹ.srst);
}

منطقی Mem::emulate_read_پہلا_ok() {
	اگر (wr_ports.خالی_خانے())
		واپس جھوٹ;
	SigSpec clk = wr_ports[0].clk;
	منطقی clk_polarity = wr_ports[0].clk_polarity;
	برائے (خودکار &پورٹ: wr_ports) {
		اگر (!پورٹ.clk_enable)
			واپس جھوٹ;
		اگر (پورٹ.clk != clk)
			واپس جھوٹ;
		اگر (پورٹ.clk_polarity != clk_polarity)
			واپس جھوٹ;
	}
	منطقی ملا_read_پہلا = جھوٹ;
	برائے (خودکار &پورٹ: rd_ports) {
		اگر (!پورٹ.clk_enable)
			واپس جھوٹ;
		اگر (پورٹ.clk != clk)
			واپس جھوٹ;
		اگر (پورٹ.clk_polarity != clk_polarity)
			واپس جھوٹ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		برائے (صحیح j = 0; j < Get_سائز(wr_ports); j++)
			اگر (!پورٹ.transparency_نقاب[j] && !پورٹ.collision_x_نقاب[j])
				ملا_read_پہلا = سچ;
	}
	واپس ملا_read_پہلا;
}

خالی Mem::emulate_read_پہلا(FfInitVals *initvals) {
	log_assert(emulate_read_پہلا_ok());
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++)
		برائے (صحیح j = 0; j < Get_سائز(wr_ports); j++)
			اگر (rd_ports[i].transparency_نقاب[j])
				emulate_transparency(j, i, initvals);
	برائے (صحیح i = 0; i < Get_سائز(rd_ports); i++)
		برائے (صحیح j = 0; j < Get_سائز(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_نقاب[j]);
			rd_ports[i].collision_x_نقاب[j] = جھوٹ;
			rd_ports[i].transparency_نقاب[j] = سچ;
		}
	برائے (خودکار &پورٹ: wr_ports) {
		Wire *new_ڈیٹا = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.ڈیٹا));
		Wire *new_addr = module->جوڑو_Wire(NEW_ID, Get_سائز(پورٹ.addr));
		خودکار compressed = پورٹ.compress_en();
		Wire *new_en = module->جوڑو_Wire(NEW_ID, Get_سائز(compressed.پہلا));
		Ff_ڈیٹا ff_ڈیٹا(module, initvals, NEW_ID);
		Ff_ڈیٹا ff_addr(module, initvals, NEW_ID);
		Ff_ڈیٹا ff_en(module, initvals, NEW_ID);
		ff_ڈیٹا.چوڑائی = Get_سائز(پورٹ.ڈیٹا);
		ff_ڈیٹا.has_clk = سچ;
		ff_ڈیٹا.sig_clk = پورٹ.clk;
		ff_ڈیٹا.pol_clk = پورٹ.clk_polarity;
		ff_ڈیٹا.sig_d = پورٹ.ڈیٹا;
		ff_ڈیٹا.sig_q = new_ڈیٹا;;
		ff_ڈیٹا.val_init = Const(State::Sx, ff_ڈیٹا.چوڑائی);
		ff_ڈیٹا.emit();
		ff_addr.چوڑائی = Get_سائز(پورٹ.addr);
		ff_addr.has_clk = سچ;
		ff_addr.sig_clk = پورٹ.clk;
		ff_addr.pol_clk = پورٹ.clk_polarity;
		ff_addr.sig_d = پورٹ.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.چوڑائی);
		ff_addr.emit();
		ff_en.چوڑائی = Get_سائز(compressed.پہلا);
		ff_en.has_clk = سچ;
		ff_en.sig_clk = پورٹ.clk;
		ff_en.pol_clk = پورٹ.clk_polarity;
		ff_en.sig_d = compressed.پہلا;
		ff_en.sig_q = new_en;;
		اگر (inits.خالی_خانے())
			ff_en.val_init = Const(State::Sx, ff_en.چوڑائی);
		ورنہ
			ff_en.val_init = Const(State::S0, ff_en.چوڑائی);
		ff_en.emit();
		پورٹ.ڈیٹا = new_ڈیٹا;
		پورٹ.addr = new_addr;
		پورٹ.en = پورٹ.decompress_en(compressed.دوسرا, new_en);
	}
}

std::pair<SigSpec, std::سمتیہ<صحیح>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::سمتیہ<صحیح> swizzle;
	Sig_ثنائی_ہندسہ پچھلا_ثنائی_ہندسہ = en[0];
	صحیح idx = 0;
	برائے (خودکار &ثنائی_ہندسہ: en) {
		اگر (ثنائی_ہندسہ != پچھلا_ثنائی_ہندسہ) {
			sig.append(ثنائی_ہندسہ);
			پچھلا_ثنائی_ہندسہ = ثنائی_ہندسہ;
			idx++;
		}
		swizzle.ڈالو_عقب(idx);
	}
	log_assert(idx + 1 == Get_سائز(sig));
	واپس {sig, swizzle};
}

SigSpec MemWr::decompress_en(ثابت std::سمتیہ<صحیح> &swizzle, SigSpec sig) {
	SigSpec res;
	برائے (صحیح i: swizzle)
		res.append(sig[i]);
	واپس res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->سائز), mem->چوڑائی)
{
	برائے(ثابت خودکار &init : mem->inits) {
		اگر(init.en.is_fully_zero()) جاری;
		log_assert(init.en.سائز() == ڈیٹا_چوڑائی_2);
		اگر(init.en.is_fully_ones())
			داخل_کرو_concatenated(init.addr.as_int(), init.ڈیٹا);
		ورنہ {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t الفاظ_2 = init.ڈیٹا.سائز() / ڈیٹا_چوڑائی_2;
			RTLIL::Const ڈیٹا = init.ڈیٹا;
			log_assert(ڈیٹا.سائز() % ڈیٹا_چوڑائی_2 == 0);
			برائے(addr_t i = 0; i < الفاظ_2; i++) {
				RTLIL::Const پچھلا_2 = (*یہ)[addr + i];
				برائے(صحیح j = 0; j < ڈیٹا_چوڑائی_2; j++)
					اگر(init.en[j] != State::S1)
						ڈیٹا.سیٹ(ڈیٹا_چوڑائی_2 * i + j, پچھلا_2[j]);
			}
			داخل_کرو_concatenated(init.addr.as_int(), ڈیٹا);
		}
	}
}

MemContents::iterator & MemContents::iterator::عامل++() {
	خودکار it = حافظہ_2->اقدار_2.بالائی_bound(_addr);
	اگر(it == حافظہ_2->اقدار_2.انتہا()) {
		حافظہ_2 = خالی_اشاریہ;
		_addr = ~(addr_t) 0;
	} ورنہ
		_addr = it->پہلا;
	واپس *یہ;
}

خالی MemContents::check() {
	log_assert(addr_چوڑائی_2 > 0 && addr_چوڑائی_2 < (صحیح)حجم(addr_t) * 8);
	log_assert(ڈیٹا_چوڑائی_2 > 0);
	log_assert(default_قدر.سائز() == ڈیٹا_چوڑائی_2);
	اگر(اقدار_2.خالی_خانے()) واپس;
	خودکار it = اقدار_2.begin();
	برائے(;;) {
		log_assert(!it->دوسرا.خالی_خانے());
		log_assert(it->دوسرا.سائز() % ڈیٹا_چوڑائی_2 == 0);
		خودکار end1 = range_انتہا(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_چوڑائی_2));
		log_assert(end1 <= (addr_t)(1<<addr_چوڑائی_2));
		اگر(++it == اقدار_2.انتہا())
			توڑو;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

منطقی MemContents::range_شامل_ہے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	واپس it != اقدار_2.انتہا() && addr - _range_begin(it) < range_سائز(it);
}


منطقی MemContents::range_شامل_ہے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتہا_addr) ثابت {
	// note that we assume begin_addr <= end_addr
	واپس it != اقدار_2.انتہا() && _range_begin(it) <= begin_addr && انتہا_addr - _range_begin(it) <= range_سائز(it);
}

منطقی MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتہا_addr) ثابت {
	اگر(it == اقدار_2.انتہا() || begin_addr >= انتہا_addr)
		واپس جھوٹ;
	خودکار top1 = range_انتہا(it) - 1;
	خودکار top2 = انتہا_addr - 1;
	واپس !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ثابت {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_چوڑائی_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	خودکار it = const_cast<std::map<addr_t, RTLIL::Const> &>(اقدار_2).بالائی_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	اگر(it == اقدار_2.begin())
		واپس it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	خودکار it_پچھلا = std::اگلا(it, -1);
	اگر(range_شامل_ہے(it_پچھلا, addr))
		واپس it_پچھلا;
	ورنہ
		واپس it;
}

RTLIL::Const MemContents::عامل[](addr_t addr) ثابت {
	خودکار it = _range_at(addr);
	اگر(range_شامل_ہے(it, addr))
		واپس it->دوسرا.نکالو_2(range_انحراف(it, addr), ڈیٹا_چوڑائی_2);
	ورنہ
		واپس default_قدر;
}

MemContents::addr_t MemContents::گنتی_range(addr_t begin_addr, addr_t انتہا_addr) ثابت {
	addr_t گنتی = 0;
	برائے(خودکار it = _range_at(begin_addr); _range_overlaps(it, begin_addr, انتہا_addr); it++) {
		خودکار پہلا = std::max(_range_begin(it), begin_addr);
		خودکار آخری = std::min(range_انتہا(it), انتہا_addr);
		گنتی += آخری - پہلا;
	}
	واپس گنتی;
}

خالی MemContents::clear_range(addr_t begin_addr, addr_t انتہا_addr) {
	اگر(begin_addr >= انتہا_addr) واپس;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	خودکار begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	خودکار انتہا_it = اقدار_2.بالائی_bound(انتہا_addr - 1);
	اگر(begin_it == انتہا_it)
		واپس; // nothing to do
	// the last iterator affected is one before the first one not affected
	خودکار آخری_it = std::اگلا(انتہا_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	اگر(range_شامل_ہے(آخری_it, انتہا_addr - 1)) {
		خودکار new_begin = انتہا_addr;
		خودکار انتہا = range_انتہا(آخری_it);
		// if there is data past the end address, preserve it by creating a new range
		اگر(new_begin != انتہا)
			انتہا_it = اقدار_2.emplace_hint(آخری_it, new_begin, آخری_it->دوسرا.نکالو_2(range_انحراف(آخری_it, new_begin), (range_انتہا(آخری_it) - new_begin) * ڈیٹا_چوڑائی_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	اگر(range_شامل_ہے(begin_it, begin_addr)) {
		خودکار new_انتہا = begin_addr;
		// if there is data before the start address, truncate but don't delete
		اگر(new_انتہا != begin_it->پہلا) {
			begin_it->دوسرا.extu(range_انحراف(begin_it, new_انتہا));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	اقدار_2.erase(begin_it, انتہا_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t انتہا_addr) {
	اگر(begin_addr >= انتہا_addr)
		واپس اقدار_2.انتہا(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	خودکار زیریں_it = begin_addr == 0 ? اقدار_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	اگر (range_شامل_ہے(زیریں_it, begin_addr, انتہا_addr))
		واپس زیریں_it;
	// find the first range containing any addr >= end_addr
	خودکار بالائی_it = _range_at(انتہا_addr);
	// check if either of the two ranges we just found touch our range
	منطقی زیریں_touch = begin_addr > 0 && range_شامل_ہے(زیریں_it, begin_addr - 1);
	منطقی بالائی_touch = range_شامل_ہے(بالائی_it, انتہا_addr);
	اگر (زیریں_touch && بالائی_touch) {
		log_assert (زیریں_it != بالائی_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		خودکار بالائی_انتہا = range_انتہا(بالائی_it);
		// make range bigger (maybe reserve here instead of resize?)
		زیریں_it->دوسرا.resize(range_انحراف(زیریں_it, بالائی_انتہا), State::Sx);
		// copy only the data beyond our range
		std::copy(range_ڈیٹا(بالائی_it, انتہا_addr), range_ڈیٹا(بالائی_it, بالائی_انتہا), range_ڈیٹا(زیریں_it, انتہا_addr));
		// keep lower_it, but delete upper_it
		اقدار_2.erase(std::اگلا(زیریں_it), std::اگلا(بالائی_it));
		واپس زیریں_it;
	} ورنہ اگر (زیریں_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		زیریں_it->دوسرا.resize(range_انحراف(زیریں_it, انتہا_addr), State::Sx);
		// keep lower_it and upper_it
		اقدار_2.erase(std::اگلا(زیریں_it), بالائی_it);
		واپس زیریں_it;
	} ورنہ اگر (بالائی_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const ڈیٹا(State::Sx, (_range_begin(بالائی_it) - begin_addr) * ڈیٹا_چوڑائی_2);
		ڈیٹا.append(std::move(بالائی_it->دوسرا));
		// delete lower_it and upper_it, then reinsert
		اقدار_2.erase(زیریں_it, std::اگلا(بالائی_it));
		واپس اقدار_2.emplace(begin_addr, std::move(ڈیٹا)).پہلا;
	} ورنہ {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		اقدار_2.erase(زیریں_it, بالائی_it);
		واپس اقدار_2.emplace(begin_addr, RTLIL::Const(State::Sx, (انتہا_addr - begin_addr) * ڈیٹا_چوڑائی_2)).پہلا;
	}
}

خالی MemContents::داخل_کرو_concatenated(addr_t addr, RTLIL::Const ثابت &اقدار) {
	addr_t الفاظ_2 = (اقدار.سائز() + ڈیٹا_چوڑائی_2 - 1) / ڈیٹا_چوڑائی_2;
	log_assert(addr < (addr_t)(1<<addr_چوڑائی_2));
	log_assert(الفاظ_2 <= (addr_t)(1<<addr_چوڑائی_2) - addr);
	خودکار it = _reserve_range(addr, addr + الفاظ_2);
	خودکار مقصد_begin = range_ڈیٹا(it, addr);
	std::copy(اقدار.begin(), اقدار.انتہا(), مقصد_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(مقصد_begin + اقدار.سائز(), مقصد_begin + الفاظ_2 * ڈیٹا_چوڑائی_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &لفظ) {
	خودکار from_انتہا = لفظ.سائز() <= ڈیٹا_چوڑائی_2 ? لفظ.انتہا() : لفظ.begin() + ڈیٹا_چوڑائی_2;
	خودکار مقصد_انتہا = std::copy(لفظ.begin(), from_انتہا, it);
	خودکار it_اگلا = std::اگلا(it, ڈیٹا_چوڑائی_2);
	std::fill(مقصد_انتہا, it_اگلا, State::S0);
	واپس it_اگلا;
}
