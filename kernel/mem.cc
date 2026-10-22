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

#ضمن "kernel/mem.h"
#ضمن "kernel/ff.h"

USING_YOSYS_NAMESPACE

فراغ Mem::remove() {
	إذا (cell) {
		module->remove(cell);
		cell = خالی_سوچک;
	}
	إذا (mem) {
		module->memories.erase(mem->name);
		مٹاؤ mem;
		mem = خالی_سوچک;
	}
	لكل (خودکار &پورٹ : rd_ports) {
		إذا (پورٹ.cell) {
			module->remove(پورٹ.cell);
			پورٹ.cell = خالی_سوچک;
		}
	}
	لكل (خودکار &پورٹ : wr_ports) {
		إذا (پورٹ.cell) {
			module->remove(پورٹ.cell);
			پورٹ.cell = خالی_سوچک;
		}
	}
	لكل (خودکار &init : inits) {
		إذا (init.cell) {
			module->remove(init.cell);
			init.cell = خالی_سوچک;
		}
	}
}

فراغ Mem::emit() {
	check();
	std::سمتیہ<صحيح> rd_بایاں;
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++) {
		خودکار &پورٹ = rd_ports[i];
		إذا (پورٹ.removed) {
			إذا (پورٹ.cell) {
				module->remove(پورٹ.cell);
			}
		} وإلا {
			rd_بایاں.ڈالو_پٹھ(i);
		}
	}
	std::سمتیہ<صحيح> wr_بایاں;
	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		إذا (پورٹ.removed) {
			إذا (پورٹ.cell) {
				module->remove(پورٹ.cell);
			}
		} وإلا {
			wr_بایاں.ڈالو_پٹھ(i);
		}
	}
	std::سمتیہ<صحيح> init_بایاں;
	لكل (صحيح i = 0; i < Get_ناپ(inits); i++) {
		خودکار &init = inits[i];
		إذا (init.removed) {
			إذا (init.cell) {
				module->remove(init.cell);
			}
		} وإلا {
			init_بایاں.ڈالو_پٹھ(i);
		}
	}
	لكل (صحيح i = 0; i < Get_ناپ(rd_بایاں); i++)
		إذا (i != rd_بایاں[i])
			std::وٹاندرا(rd_ports[i], rd_ports[rd_بایاں[i]]);
	rd_ports.resize(Get_ناپ(rd_بایاں));
	لكل (صحيح i = 0; i < Get_ناپ(wr_بایاں); i++)
		إذا (i != wr_بایاں[i])
			std::وٹاندرا(wr_ports[i], wr_ports[wr_بایاں[i]]);
	wr_ports.resize(Get_ناپ(wr_بایاں));
	لكل (صحيح i = 0; i < Get_ناپ(init_بایاں); i++)
		إذا (i != init_بایاں[i])
			std::وٹاندرا(inits[i], inits[init_بایاں[i]]);
	inits.resize(Get_ناپ(init_بایاں));

	لكل (خودکار &پورٹ : rd_ports) {
		لكل (صحيح i = 0; i < Get_ناپ(wr_بایاں); i++) {
			پورٹ.transparency_نقاب[i] = پورٹ.transparency_نقاب[wr_بایاں[i]];
			پورٹ.collision_x_نقاب[i] = پورٹ.collision_x_نقاب[wr_بایاں[i]];
		}
		پورٹ.transparency_نقاب.resize(Get_ناپ(wr_بایاں));
		پورٹ.collision_x_نقاب.resize(Get_ناپ(wr_بایاں));
	}
	لكل (خودکار &پورٹ : wr_ports) {
		لكل (صحيح i = 0; i < Get_ناپ(wr_بایاں); i++)
			پورٹ.ترجیح_نقاب[i] = پورٹ.ترجیح_نقاب[wr_بایاں[i]];
		پورٹ.ترجیح_نقاب.resize(Get_ناپ(wr_بایاں));
	}

	إذا (packed) {
		إذا (mem) {
			module->memories.erase(mem->name);
			مٹاؤ mem;
			mem = خالی_سوچک;
		}
		إذا (!cell) {
			إذا (memid.خالی_تھاواں())
				memid = PooledName(module->design, module->twines().جوڑو(NEW_ID));
			cell = module->جوڑو_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->گن = گن;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::چوڑائی_2] = Const(چوڑائی);
		cell->parameters[ID::وتھ_2] = Const(شروع_وتھ);
		cell->parameters[ID::ناپ_2] = Const(ناپ);
		SigSpec rd_clk, rd_en, rd_addr, rd_انکڑے;
		SigSpec wr_clk, wr_en, wr_addr, wr_انکڑے;
		SigSpec rd_arst, rd_srst;
		صحيح abits = 0;
		لكل (خودکار &پورٹ : rd_ports)
			abits = std::max(abits, Get_ناپ(پورٹ.addr));
		لكل (خودکار &پورٹ : wr_ports)
			abits = std::max(abits, Get_ناپ(پورٹ.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::سمتیہ<صحيح> wr_پورٹ_xlat;
		لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++)
			لكل (صحيح j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_پورٹ_xlat.ڈالو_پٹھ(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_نقاب_builder;
		Const::Builder rd_collision_x_نقاب_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_قدر_builder;
		Const::Builder rd_srst_قدر_builder;
		Const::Builder rd_init_قدر_builder;
		لكل (خودکار &پورٹ : rd_ports) {
			لكل (خودکار attr: پورٹ.گن)
				إذا (!cell->has_attribute(attr.پہلا))
					cell->گن.داخل_کرو(attr);
			إذا (پورٹ.cell) {
				module->remove(پورٹ.cell);
				پورٹ.cell = خالی_سوچک;
			}
			لكل (صحيح sub = 0; sub < (1 << پورٹ.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ڈالو_پٹھ(State(sub != 0));
				rd_clk_enable_builder.ڈالو_پٹھ(State(پورٹ.clk_enable));
				rd_clk_polarity_builder.ڈالو_پٹھ(State(پورٹ.clk_polarity));
				rd_ce_over_srst_builder.ڈالو_پٹھ(State(پورٹ.ce_over_srst));
				rd_clk.append(پورٹ.clk);
				rd_arst.append(پورٹ.arst);
				rd_srst.append(پورٹ.srst);
				rd_en.append(پورٹ.en);
				SigSpec addr = پورٹ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				rd_addr.append(addr);
				log_assert(Get_ناپ(addr) == abits);
				لكل (خودکار idx : wr_پورٹ_xlat) {
					rd_transparency_نقاب_builder.ڈالو_پٹھ(State(منطقي(پورٹ.transparency_نقاب[idx])));
					rd_collision_x_نقاب_builder.ڈالو_پٹھ(State(منطقي(پورٹ.collision_x_نقاب[idx])));
				}
			}
			rd_انکڑے.append(پورٹ.انکڑے);
			لكل (خودکار ثنائی_ہندسہ : پورٹ.arst_قدر)
				rd_arst_قدر_builder.ڈالو_پٹھ(ثنائی_ہندسہ);
			لكل (خودکار ثنائی_ہندسہ : پورٹ.srst_قدر)
				rd_srst_قدر_builder.ڈالو_پٹھ(ثنائی_ہندسہ);
			لكل (خودکار ثنائی_ہندسہ : پورٹ.init_قدر)
				rd_init_قدر_builder.ڈالو_پٹھ(ثنائی_ہندسہ);
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
		إذا (rd_ports.خالی_تھاواں()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_قدر = State::S0;
			rd_srst_قدر = State::S0;
			rd_init_قدر = State::S0;
		}
		إذا (rd_ports.خالی_تھاواں() || wr_ports.خالی_تھاواں()) {
			rd_transparency_نقاب = State::S0;
			rd_collision_x_نقاب = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_ناپ(rd_clk));
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
		cell->سیٹ_پورٹ(ID::RD_انکڑے, rd_انکڑے);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ترجیح_نقاب_builder;
		لكل (خودکار &پورٹ : wr_ports) {
			لكل (خودکار attr: پورٹ.گن)
				إذا (!cell->has_attribute(attr.پہلا))
					cell->گن.داخل_کرو(attr);
			إذا (پورٹ.cell) {
				module->remove(پورٹ.cell);
				پورٹ.cell = خالی_سوچک;
			}
			لكل (صحيح sub = 0; sub < (1 << پورٹ.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ڈالو_پٹھ(State(sub != 0));
				wr_clk_enable_builder.ڈالو_پٹھ(State(پورٹ.clk_enable));
				wr_clk_polarity_builder.ڈالو_پٹھ(State(پورٹ.clk_polarity));
				wr_clk.append(پورٹ.clk);
				لكل (خودکار idx : wr_پورٹ_xlat)
					wr_ترجیح_نقاب_builder.ڈالو_پٹھ(State(منطقي(پورٹ.ترجیح_نقاب[idx])));
				SigSpec addr = پورٹ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				wr_addr.append(addr);
				log_assert(Get_ناپ(addr) == abits);
			}
			wr_en.append(پورٹ.en);
			wr_انکڑے.append(پورٹ.انکڑے);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ترجیح_نقاب = wr_ترجیح_نقاب_builder.build();
		إذا (wr_ports.خالی_تھاواں()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ترجیح_نقاب = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_ناپ(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ترجیح_نقاب] = wr_ترجیح_نقاب;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->سیٹ_پورٹ(ID::WR_CLK, wr_clk);
		cell->سیٹ_پورٹ(ID::WR_EN, wr_en);
		cell->سیٹ_پورٹ(ID::WR_ADDR, wr_addr);
		cell->سیٹ_پورٹ(ID::WR_انکڑے, wr_انکڑے);
		لكل (خودکار &init : inits) {
			لكل (خودکار attr: init.گن)
				إذا (!cell->has_attribute(attr.پہلا))
					cell->گن.داخل_کرو(attr);
			إذا (init.cell) {
				module->remove(init.cell);
				init.cell = خالی_سوچک;
			}
		}
		cell->parameters[ID::INIT] = get_init_انکڑے();
	} وإلا {
		إذا (cell) {
			module->remove(cell);
			cell = خالی_سوچک;
		}
		إذا (!mem) {
			إذا (memid.خالی_تھاواں())
				memid = PooledName(module->design, module->twines().جوڑو(NEW_ID));
			mem = module->جوڑو_حافظہ(memid);
		}
		mem->چوڑائی = چوڑائی;
		mem->شروع_وتھ = شروع_وتھ;
		mem->ناپ = ناپ;
		mem->گن = گن;
		لكل (خودکار &پورٹ : rd_ports) {
			إذا (!پورٹ.cell)
				پورٹ.cell = module->جوڑو_Cell(NEW_ID, ID($memrd_v2));
			پورٹ.cell->type = ID($memrd_v2);
			پورٹ.cell->گن = پورٹ.گن;
			پورٹ.cell->parameters[ID::MEMID] = memid.str();
			پورٹ.cell->parameters[ID::ABITS] = Get_ناپ(پورٹ.addr);
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
			پورٹ.cell->سیٹ_پورٹ(ID::انکڑے_2, پورٹ.انکڑے);
		}
		صحيح idx = 0;
		لكل (خودکار &پورٹ : wr_ports) {
			إذا (!پورٹ.cell)
				پورٹ.cell = module->جوڑو_Cell(NEW_ID, ID($memwr_v2));
			پورٹ.cell->type = ID($memwr_v2);
			پورٹ.cell->گن = پورٹ.گن;
			إذا (پورٹ.cell->parameters.گنتی(ID::ترجیح))
				پورٹ.cell->parameters.erase(ID::ترجیح);
			پورٹ.cell->parameters[ID::MEMID] = memid.str();
			پورٹ.cell->parameters[ID::ABITS] = Get_ناپ(پورٹ.addr);
			پورٹ.cell->parameters[ID::چوڑائی_2] = چوڑائی << پورٹ.wide_log2;
			پورٹ.cell->parameters[ID::CLK_ENABLE] = پورٹ.clk_enable;
			پورٹ.cell->parameters[ID::CLK_POLARITY] = پورٹ.clk_polarity;
			پورٹ.cell->parameters[ID::PORTID] = idx++;
			پورٹ.cell->parameters[ID::ترجیح_نقاب_2] = پورٹ.ترجیح_نقاب;
			پورٹ.cell->سیٹ_پورٹ(ID::CLK, پورٹ.clk);
			پورٹ.cell->سیٹ_پورٹ(ID::EN, پورٹ.en);
			پورٹ.cell->سیٹ_پورٹ(ID::ADDR, پورٹ.addr);
			پورٹ.cell->سیٹ_پورٹ(ID::انکڑے_2, پورٹ.انکڑے);
		}
		idx = 0;
		لكل (خودکار &init : inits) {
			منطقي v2 = !init.en.is_fully_ones();
			إذا (!init.cell)
				init.cell = module->جوڑو_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			وإلا {
				إذا (!v2)
					init.cell->unset_پورٹ(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->گن = init.گن;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_ناپ(init.addr);
			init.cell->parameters[ID::چوڑائی_2] = چوڑائی;
			init.cell->parameters[ID::الفاظ] = Get_ناپ(init.انکڑے) / چوڑائی;
			init.cell->parameters[ID::ترجیح] = idx++;
			init.cell->سیٹ_پورٹ(ID::ADDR, init.addr);
			init.cell->سیٹ_پورٹ(ID::انکڑے_2, init.انکڑے);
			إذا (v2)
				init.cell->سیٹ_پورٹ(ID::EN, init.en);
		}
	}
}

فراغ Mem::clear_inits() {
	لكل (خودکار &init : inits)
		init.removed = سچ;
}

فراغ Mem::coalesce_inits() {
	// start address -> end address
	std::map<صحيح, صحيح> chunks;
	// Figure out chunk boundaries.
	لكل (خودکار &init : inits) {
		إذا (init.removed)
			استمر;
		منطقي valid = خطأ;
		لكل (خودکار ثنائی_ہندسہ : init.en)
			إذا (ثنائی_ہندسہ == State::S1)
				valid = سچ;
		إذا (!valid) {
			init.removed = سچ;
			استمر;
		}
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_ناپ(init.انکڑے) / چوڑائی;
		خودکار it_e = chunks.اتلا_bound(addr_e);
		خودکار it = it_e;
		طالما (it != chunks.begin()) {
			--it;
			إذا (it->دوجا < addr) {
				++it;
				اكسر;
			}
		}
		إذا (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} وإلا {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			إذا (it->پہلا < addr)
				addr = it->پہلا;
			خودکار it_آخری = it_e;
			it_آخری--;
			إذا (it_آخری->دوجا > addr_e)
				addr_e = it_آخری->دوجا;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<صحيح, std::سمتیہ<صحيح>> inits_by_chunk;
	لكل (صحيح i = 0; i < Get_ناپ(inits); i++) {
		خودکار &init = inits[i];
		إذا (init.removed)
			استمر;
		خودکار it = chunks.اتلا_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->پہلا].ڈالو_پٹھ(i);
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_ناپ(init.انکڑے) / چوڑائی;
		log_assert(addr >= it->پہلا && addr_e <= it->دوجا);
	}
	// Process each chunk.
	لكل (خودکار &it : inits_by_chunk) {
		صحيح caddr = it.پہلا;
		صحيح caddr_e = chunks[caddr];
		خودکار &chunk_inits = it.دوجا;
		إذا (Get_ناپ(chunk_inits) == 1) {
			خودکار &init = inits[chunk_inits[0]];
			إذا (!init.en.is_fully_ones()) {
				لكل (صحيح i = 0; i < Get_ناپ(init.انکڑے); i++)
					إذا (init.en[i % چوڑائی] != State::S1)
						init.انکڑے.سیٹ(i, State::Sx);
				init.en = Const(State::S1, چوڑائی);
			}
			استمر;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * چوڑائی);
		لكل (صحيح idx : chunk_inits) {
			خودکار &init = inits[idx];
			صحيح وتھ = (init.addr.as_int() - caddr) * چوڑائی;
			log_assert(وتھ >= 0);
			log_assert(وتھ + Get_ناپ(init.انکڑے) <= Get_ناپ(cdata));
			لكل (صحيح i = 0; i < Get_ناپ(init.انکڑے); i++)
				إذا (init.en[i % چوڑائی] == State::S1)
					cdata.سیٹ(i+وتھ, init.انکڑے[i]);
			init.removed = سچ;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.انکڑے = cdata;
		new_init.en = Const(State::S1, چوڑائی);
		inits.ڈالو_پٹھ(new_init);
	}
}

Const Mem::get_init_انکڑے() ثابت {
	Const init_انکڑے(State::Sx, چوڑائی * ناپ);
	لكل (خودکار &init : inits) {
		إذا (init.removed)
			استمر;
		صحيح وتھ = (init.addr.as_int() - شروع_وتھ) * چوڑائی;
		لكل (صحيح i = 0; i < Get_ناپ(init.انکڑے); i++)
			إذا (0 <= i+وتھ && i+وتھ < Get_ناپ(init_انکڑے) && init.en[i % چوڑائی] == State::S1)
				init_انکڑے.سیٹ(i+وتھ, init.انکڑے[i]);
	}
	أرجع init_انکڑے;
}

فراغ Mem::check() {
	صحيح max_wide_log2 = 0;
	لكل (خودکار &پورٹ : rd_ports) {
		إذا (پورٹ.removed)
			استمر;
		log_assert(Get_ناپ(پورٹ.clk) == 1);
		log_assert(Get_ناپ(پورٹ.en) == 1);
		log_assert(Get_ناپ(پورٹ.arst) == 1);
		log_assert(Get_ناپ(پورٹ.srst) == 1);
		log_assert(Get_ناپ(پورٹ.addr) >= پورٹ.wide_log2);
		log_assert(Get_ناپ(پورٹ.انکڑے) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_ناپ(پورٹ.init_قدر) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_ناپ(پورٹ.arst_قدر) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_ناپ(پورٹ.srst_قدر) == (چوڑائی << پورٹ.wide_log2));
		إذا (!پورٹ.clk_enable) {
			log_assert(پورٹ.en == State::S1);
			log_assert(پورٹ.arst == State::S0);
			log_assert(پورٹ.srst == State::S0);
		}
		لكل (صحيح j = 0; j < پورٹ.wide_log2; j++) {
			log_assert(پورٹ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٹ.wide_log2);
		log_assert(Get_ناپ(پورٹ.transparency_نقاب) == Get_ناپ(wr_ports));
		log_assert(Get_ناپ(پورٹ.collision_x_نقاب) == Get_ناپ(wr_ports));
		لكل (صحيح j = 0; j < Get_ناپ(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			إذا ((پورٹ.transparency_نقاب[j] || پورٹ.collision_x_نقاب[j]) && !wport.removed) {
				log_assert(پورٹ.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(پورٹ.clk == wport.clk);
				log_assert(پورٹ.clk_polarity == wport.clk_polarity);
			}
			log_assert(!پورٹ.transparency_نقاب[j] || !پورٹ.collision_x_نقاب[j]);
		}
	}
	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		إذا (پورٹ.removed)
			استمر;
		log_assert(Get_ناپ(پورٹ.clk) == 1);
		log_assert(Get_ناپ(پورٹ.en) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_ناپ(پورٹ.انکڑے) == (چوڑائی << پورٹ.wide_log2));
		log_assert(Get_ناپ(پورٹ.addr) >= پورٹ.wide_log2);
		لكل (صحيح j = 0; j < پورٹ.wide_log2; j++) {
			log_assert(پورٹ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٹ.wide_log2);
		log_assert(Get_ناپ(پورٹ.ترجیح_نقاب) == Get_ناپ(wr_ports));
		لكل (صحيح j = 0; j < Get_ناپ(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			إذا (پورٹ.ترجیح_نقاب[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(پورٹ.clk_enable == wport.clk_enable);
				إذا (پورٹ.clk_enable) {
					log_assert(پورٹ.clk == wport.clk);
					log_assert(پورٹ.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	صحيح نقاب = (1 << max_wide_log2) - 1;
	log_assert(!(شروع_وتھ & نقاب));
	log_assert(!(ناپ & نقاب));
	log_assert(چوڑائی != 0);
}

نطاق {

	بنية Mem_اشاریہ {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_اشاریہ (Module *module) {
			لكل (خودکار cell: module->cells()) {
				إذا (cell->type.وچ(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
				وإلا إذا (cell->type.وچ(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
				وإلا إذا (cell->type.وچ(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].داخل_کرو(cell);
			}
		}
	};

	Mem mem_from_حافظہ(Module *module, RTLIL::حافظہ *mem, ثابت Mem_اشاریہ &اشاریہ) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->چوڑائی, mem->شروع_وتھ, mem->ناپ);
		res.packed = خطأ;
		res.mem = mem;
		res.گن = mem->گن;
		std::سمتیہ<منطقي> rd_transparent;
		std::سمتیہ<صحيح> wr_portid;
		إذا (اشاریہ.rd_ports.گنتی(memid)) {
			لكل (خودکار cell : اشاریہ.rd_ports.at(memid)) {
				MemRd mrd;
				منطقي is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.گن = cell->گن;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_پورٹ(ID::CLK);
				mrd.en = cell->get_پورٹ(ID::EN);
				mrd.addr = cell->get_پورٹ(ID::ADDR);
				mrd.انکڑے = cell->get_پورٹ(ID::انکڑے_2);
				mrd.wide_log2 = ceil_log2(Get_ناپ(mrd.انکڑے) / mem->چوڑائی);
				منطقي transparent = خطأ;
				إذا (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = خطأ;
					mrd.arst_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.srst_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.init_قدر = Const(State::Sx, mem->چوڑائی << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					إذا (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = خطأ;
						إذا (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} وإلا {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_قدر = cell->parameters.at(ID::ARST_قدر);
					mrd.srst_قدر = cell->parameters.at(ID::SRST_قدر);
					mrd.init_قدر = cell->parameters.at(ID::INIT_قدر);
					mrd.arst = cell->get_پورٹ(ID::ARST);
					mrd.srst = cell->get_پورٹ(ID::SRST);
				}
				res.rd_ports.ڈالو_پٹھ(mrd);
				rd_transparent.ڈالو_پٹھ(transparent);
			}
		}
		إذا (اشاریہ.wr_ports.گنتی(memid)) {
			std::سمتیہ<std::pair<صحيح, MemWr>> ports;
			لكل (خودکار cell : اشاریہ.wr_ports.at(memid)) {
				MemWr mwr;
				منطقي is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.گن = cell->گن;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_پورٹ(ID::CLK);
				mwr.en = cell->get_پورٹ(ID::EN);
				mwr.addr = cell->get_پورٹ(ID::ADDR);
				mwr.انکڑے = cell->get_پورٹ(ID::انکڑے_2);
				mwr.wide_log2 = ceil_log2(Get_ناپ(mwr.انکڑے) / mem->چوڑائی);
				ports.ڈالو_پٹھ(std::make_pair(cell->parameters.at(is_compat ? ID::ترجیح : ID::PORTID).as_int(), mwr));
			}
			std::ترتیب_دیو(ports.begin(), ports.سرا(), [](ثابت std::pair<صحيح, MemWr> &a, ثابت std::pair<صحيح, MemWr> &b) { أرجع a.پہلا < b.پہلا; });
			لكل (خودکار &it : ports) {
				res.wr_ports.ڈالو_پٹھ(it.دوجا);
				wr_portid.ڈالو_پٹھ(it.پہلا);
			}
			لكل (صحيح i = 0; i < Get_ناپ(res.wr_ports); i++) {
				خودکار &پورٹ = res.wr_ports[i];
				منطقي is_compat = پورٹ.cell->type == ID($memwr);
				إذا (is_compat) {
					پورٹ.ترجیح_نقاب.resize(Get_ناپ(res.wr_ports));
					لكل (صحيح j = 0; j < i; j++) {
						خودکار &oport = res.wr_ports[j];
						إذا (پورٹ.clk_enable != oport.clk_enable)
							استمر;
						إذا (پورٹ.clk_enable && پورٹ.clk != oport.clk)
							استمر;
						إذا (پورٹ.clk_enable && پورٹ.clk_polarity != oport.clk_polarity)
							استمر;
						پورٹ.ترجیح_نقاب[j] = سچ;
					}
				} وإلا {
					Const orig_prio_نقاب = پورٹ.cell->parameters.at(ID::ترجیح_نقاب_2);
					لكل (صحيح orig_portid : wr_portid) {
						منطقي has_prio = orig_portid < Get_ناپ(orig_prio_نقاب) && orig_prio_نقاب[orig_portid] == State::S1;
						پورٹ.ترجیح_نقاب.ڈالو_پٹھ(has_prio);
					}
				}
			}
		}
		إذا (اشاریہ.inits.گنتی(memid)) {
			std::سمتیہ<std::pair<صحيح, MemInit>> inits;
			لكل (خودکار cell : اشاریہ.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.گن = cell->گن;
				خودکار addr = cell->get_پورٹ(ID::ADDR);
				خودکار انکڑے = cell->get_پورٹ(ID::انکڑے_2);
				إذا (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				إذا (!انکڑے.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(انکڑے), cell);
				init.addr = addr.as_const();
				init.انکڑے = انکڑے.as_const();
				إذا (cell->type == ID($meminit_v2)) {
					خودکار en = cell->get_پورٹ(ID::EN);
					إذا (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} وإلا {
					init.en = RTLIL::Const(State::S1, mem->چوڑائی);
				}
				inits.ڈالو_پٹھ(std::make_pair(cell->parameters.at(ID::ترجیح).as_int(), init));
			}
			std::ترتیب_دیو(inits.begin(), inits.سرا(), [](ثابت std::pair<صحيح, MemInit> &a, ثابت std::pair<صحيح, MemInit> &b) { أرجع a.پہلا < b.پہلا; });
			لكل (خودکار &it : inits)
				res.inits.ڈالو_پٹھ(it.دوجا);
		}
		لكل (صحيح i = 0; i < Get_ناپ(res.rd_ports); i++) {
			خودکار &پورٹ = res.rd_ports[i];
			منطقي is_compat = پورٹ.cell->type == ID($memrd);
			إذا (is_compat) {
				پورٹ.transparency_نقاب.resize(Get_ناپ(res.wr_ports));
				پورٹ.collision_x_نقاب.resize(Get_ناپ(res.wr_ports));
				إذا (!rd_transparent[i])
					استمر;
				إذا (!پورٹ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_ناپ(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورٹ.clk != wport.clk)
						استمر;
					إذا (پورٹ.clk_polarity != wport.clk_polarity)
						استمر;
					پورٹ.transparency_نقاب[j] = سچ;
				}
			} وإلا {
				Const orig_trans_نقاب = پورٹ.cell->parameters.at(ID::TRANSPARENCY_نقاب);
				Const orig_cx_نقاب = پورٹ.cell->parameters.at(ID::COLLISION_X_نقاب);
				لكل (صحيح orig_portid : wr_portid) {
					پورٹ.transparency_نقاب.ڈالو_پٹھ(orig_portid < Get_ناپ(orig_trans_نقاب) && orig_trans_نقاب[orig_portid] == State::S1);
					پورٹ.collision_x_نقاب.ڈالو_پٹھ(orig_portid < Get_ناپ(orig_cx_نقاب) && orig_cx_نقاب[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		أرجع res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().جوڑو(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::چوڑائی_2).as_int(),
			cell->parameters.at(ID::وتھ_2).as_int(),
			cell->parameters.at(ID::ناپ_2).as_int()
		);
		منطقي is_compat = cell->type == ID($mem);
		صحيح abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = سچ;
		res.cell = cell;
		res.گن = cell->گن;
		Const &init = cell->parameters.at(ID::INIT);
		إذا (!init.is_fully_undef()) {
			صحيح pos = 0;
			طالما (pos < res.ناپ) {
				Const لفظ = init.کڈھو(pos * res.چوڑائی, res.چوڑائی, State::Sx);
				إذا (لفظ.is_fully_undef()) {
					pos++;
				} وإلا {
					صحيح epos;
					لكل (epos = pos; epos < res.ناپ; epos++) {
						Const eword = init.کڈھو(epos * res.چوڑائی, res.چوڑائی, State::Sx);
						إذا (eword.is_fully_undef())
							اكسر;
					}
					MemInit minit;
					minit.addr = res.شروع_وتھ + pos;
					minit.انکڑے = init.کڈھو(pos * res.چوڑائی, (epos - pos) * res.چوڑائی, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.چوڑائی);
					res.inits.ڈالو_پٹھ(minit);
					pos = epos;
				}
			}
		}
		صحيح n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		صحيح n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		لكل (صحيح i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).کڈھو(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).کڈھو(i, 1).as_bool();
			mrd.clk = cell->get_پورٹ(ID::RD_CLK).کڈھو(i, 1);
			mrd.en = cell->get_پورٹ(ID::RD_EN).کڈھو(i, 1);
			mrd.addr = cell->get_پورٹ(ID::RD_ADDR).کڈھو(i * abits, abits);
			mrd.انکڑے = cell->get_پورٹ(ID::RD_انکڑے).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			إذا (is_compat) {
				mrd.ce_over_srst = خطأ;
				mrd.arst_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.srst_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.init_قدر = Const(State::Sx, res.چوڑائی << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} وإلا {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).کڈھو(i, 1).as_bool();
				mrd.arst_قدر = cell->parameters.at(ID::RD_ARST_قدر).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.srst_قدر = cell->parameters.at(ID::RD_SRST_قدر).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.init_قدر = cell->parameters.at(ID::RD_INIT_قدر).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
				mrd.arst = cell->get_پورٹ(ID::RD_ARST).کڈھو(i, 1);
				mrd.srst = cell->get_پورٹ(ID::RD_SRST).کڈھو(i, 1);
			}
			إذا (!is_compat) {
				Const transparency_نقاب = cell->parameters.at(ID::RD_TRANSPARENCY_نقاب).کڈھو(i * n_wr_ports, n_wr_ports);
				Const collision_x_نقاب = cell->parameters.at(ID::RD_COLLISION_X_نقاب).کڈھو(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_نقاب.ڈالو_پٹھ(transparency_نقاب[j] == State::S1);
						mrd.collision_x_نقاب.ڈالو_پٹھ(collision_x_نقاب[j] == State::S1);
					}
			}
			res.rd_ports.ڈالو_پٹھ(mrd);
		}
		لكل (صحيح i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).کڈھو(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).کڈھو(i, 1).as_bool();
			mwr.clk = cell->get_پورٹ(ID::WR_CLK).کڈھو(i, 1);
			mwr.en = cell->get_پورٹ(ID::WR_EN).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			mwr.addr = cell->get_پورٹ(ID::WR_ADDR).کڈھو(i * abits, abits);
			mwr.انکڑے = cell->get_پورٹ(ID::WR_انکڑے).کڈھو(i * res.چوڑائی, (ni - i) * res.چوڑائی);
			إذا (!is_compat) {
				Const ترجیح_نقاب = cell->parameters.at(ID::WR_ترجیح_نقاب).کڈھو(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1)
						mwr.ترجیح_نقاب.ڈالو_پٹھ(ترجیح_نقاب[j] == State::S1);
			}
			res.wr_ports.ڈالو_پٹھ(mwr);
		}
		إذا (is_compat) {
			لكل (صحيح i = 0; i < Get_ناپ(res.wr_ports); i++) {
				خودکار &پورٹ = res.wr_ports[i];
				پورٹ.ترجیح_نقاب.resize(Get_ناپ(res.wr_ports));
				لكل (صحيح j = 0; j < i; j++) {
					خودکار &oport = res.wr_ports[j];
					إذا (پورٹ.clk_enable != oport.clk_enable)
						استمر;
					إذا (پورٹ.clk_enable && پورٹ.clk != oport.clk)
						استمر;
					إذا (پورٹ.clk_enable && پورٹ.clk_polarity != oport.clk_polarity)
						استمر;
					پورٹ.ترجیح_نقاب[j] = سچ;
				}
			}
			لكل (صحيح i = 0; i < Get_ناپ(res.rd_ports); i++) {
				خودکار &پورٹ = res.rd_ports[i];
				پورٹ.transparency_نقاب.resize(Get_ناپ(res.wr_ports));
				پورٹ.collision_x_نقاب.resize(Get_ناپ(res.wr_ports));
				إذا (!cell->parameters.at(ID::RD_TRANSPARENT).کڈھو(i, 1).as_bool())
					استمر;
				إذا (!پورٹ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_ناپ(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورٹ.clk != wport.clk)
						استمر;
					إذا (پورٹ.clk_polarity != wport.clk_polarity)
						استمر;
					پورٹ.transparency_نقاب[j] = سچ;
				}
			}
		}
		res.check();
		أرجع res;
	}

}

std::سمتیہ<Mem> Mem::get_all_memories(Module *module) {
	std::سمتیہ<Mem> res;
	Mem_اشاریہ اشاریہ(module);
	لكل (خودکار it: module->memories) {
		res.ڈالو_پٹھ(mem_from_حافظہ(module, it.دوجا, اشاریہ));
	}
	لكل (خودکار cell: module->cells()) {
		إذا (cell->type.وچ(ID($mem), ID($mem_v2)))
			res.ڈالو_پٹھ(mem_from_cell(cell));
	}
	أرجع res;
}

std::سمتیہ<Mem> Mem::get_selected_memories(Module *module) {
	std::سمتیہ<Mem> res;
	Mem_اشاریہ اشاریہ(module);
	لكل (خودکار it: module->memories) {
		إذا (module->design->selected(module, it.دوجا))
			res.ڈالو_پٹھ(mem_from_حافظہ(module, it.دوجا, اشاریہ));
	}
	لكل (خودکار cell: module->selected_cells()) {
		إذا (cell->type.وچ(ID($mem), ID($mem_v2)))
			res.ڈالو_پٹھ(mem_from_cell(cell));
	}
	أرجع res;
}

Cell *Mem::کڈھو_rdff(صحيح idx, FfInitVals *initvals) {
	MemRd &پورٹ = rd_ports[idx];

	إذا (!پورٹ.clk_enable)
		أرجع خالی_سوچک;

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
	منطقي trans_use_addr = سچ;
	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++)
		إذا (!پورٹ.transparency_نقاب[i] && !wr_ports[i].removed)
			trans_use_addr = خطأ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	إذا (Get_ناپ(wr_ports) == 0)
		trans_use_addr = خطأ;

	إذا (پورٹ.en != State::S1 || پورٹ.srst != State::S0 || پورٹ.arst != State::S0 || !پورٹ.init_قدر.is_fully_undef())
		trans_use_addr = خطأ;

	إذا (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		صحيح چوڑائی = 0;
		لكل (صحيح i = 0; i < Get_ناپ(پورٹ.addr); i++)
			إذا (پورٹ.addr[i].wire)
				چوڑائی++;

		إذا (چوڑائی)
		{
			SigSpec sig_q = module->جوڑو_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), چوڑائی);
			SigSpec sig_d;

			صحيح pos = 0;
			لكل (صحيح i = 0; i < Get_ناپ(پورٹ.addr); i++)
				إذا (پورٹ.addr[i].wire) {
					sig_d.append(پورٹ.addr[i]);
					پورٹ.addr[i] = sig_q[pos++];
				}

			c = module->جوڑو_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), پورٹ.clk, sig_d, sig_q, پورٹ.clk_polarity);
		} وإلا {
			c = خالی_سوچک;
		}
	}
	وإلا
	{
		log_assert(پورٹ.arst == State::S0 || پورٹ.srst == State::S0);

		SigSpec async_d = module->جوڑو_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_ناپ(پورٹ.انکڑے));
		SigSpec sig_d = async_d;

		لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
			خودکار &wport = wr_ports[i];
			إذا (wport.removed)
				استمر;
			إذا (پورٹ.transparency_نقاب[i] || پورٹ.collision_x_نقاب[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == پورٹ.clk);
				log_assert(wport.clk_enable == پورٹ.clk_enable);
				صحيح min_wide_log2 = std::min(پورٹ.wide_log2, wport.wide_log2);
				صحيح max_wide_log2 = std::max(پورٹ.wide_log2, wport.wide_log2);
				منطقي wide_write = wport.wide_log2 > پورٹ.wide_log2;
				لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = پورٹ.addr;
					SigSpec waddr = wport.addr;
					إذا (wide_write)
						waddr = wport.sub_addr(sub);
					وإلا
						raddr = پورٹ.sub_addr(sub);
					SigSpec addr_eq;
					إذا (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					صحيح pos = 0;
					صحيح ewidth = چوڑائی << min_wide_log2;
					صحيح wsub = wide_write ? sub : 0;
					صحيح rsub = wide_write ? 0 : sub;
					طالما (pos < ewidth) {
						صحيح epos = pos;
						طالما (epos < ewidth && wport.en[epos + wsub * چوڑائی] == wport.en[pos + wsub * چوڑائی])
							epos++;
						SigSpec cur = sig_d.کڈھو(pos + rsub * چوڑائی, epos-pos);
						SigSpec other = پورٹ.transparency_نقاب[i] ? wport.انکڑے.کڈھو(pos + wsub * چوڑائی, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						إذا (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * چوڑائی], addr_eq);
						وإلا
							cond = wport.en[pos + wsub * چوڑائی];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * چوڑائی, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().جوڑو(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_انکڑے ff(module, initvals, name);
		ff.چوڑائی = Get_ناپ(پورٹ.انکڑے);
		ff.has_clk = سچ;
		ff.sig_clk = پورٹ.clk;
		ff.pol_clk = پورٹ.clk_polarity;
		إذا (پورٹ.en != State::S1) {
			ff.has_ce = سچ;
			ff.pol_ce = سچ;
			ff.sig_ce = پورٹ.en;
		}
		إذا (پورٹ.arst != State::S0) {
			ff.has_arst = سچ;
			ff.pol_arst = سچ;
			ff.sig_arst = پورٹ.arst;
			ff.val_arst = پورٹ.arst_قدر;
		}
		إذا (پورٹ.srst != State::S0) {
			ff.has_srst = سچ;
			ff.pol_srst = سچ;
			ff.sig_srst = پورٹ.srst;
			ff.val_srst = پورٹ.srst_قدر;
			ff.ce_over_srst = ff.has_ce && پورٹ.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = پورٹ.انکڑے;
		ff.val_init = پورٹ.init_قدر;
		پورٹ.انکڑے = async_d;
		c = ff.emit();
	}

	إذا (c)
		لوگارتھم("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	پورٹ.en = State::S1;
	پورٹ.clk = State::S0;
	پورٹ.arst = State::S0;
	پورٹ.srst = State::S0;
	پورٹ.clk_enable = خطأ;
	پورٹ.clk_polarity = سچ;
	پورٹ.ce_over_srst = خطأ;
	پورٹ.arst_قدر = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));
	پورٹ.srst_قدر = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));
	پورٹ.init_قدر = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));

	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
		پورٹ.transparency_نقاب[i] = خطأ;
		پورٹ.collision_x_نقاب[i] = خطأ;
	}

	أرجع c;
}

فراغ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::سمتیہ<MemRd> new_rd_ports;
	std::سمتیہ<MemWr> new_wr_ports;
	std::سمتیہ<std::pair<صحيح, صحيح>> new_rd_map;
	std::سمتیہ<std::pair<صحيح, صحيح>> new_wr_map;
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++) {
		خودکار &پورٹ = rd_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورٹ.wide_log2); sub++) {
			new_rd_map.ڈالو_پٹھ(std::make_pair(i, sub));
		}
	}
	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
		خودکار &پورٹ = wr_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورٹ.wide_log2); sub++) {
			new_wr_map.ڈالو_پٹھ(std::make_pair(i, sub));
		}
	}
	لكل (خودکار &it : new_rd_map) {
		MemRd &orig = rd_ports[it.پہلا];
		MemRd پورٹ = orig;
		إذا (it.دوجا != 0)
			پورٹ.cell = خالی_سوچک;
		إذا (پورٹ.wide_log2) {
			پورٹ.انکڑے = پورٹ.انکڑے.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.init_قدر = پورٹ.init_قدر.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.arst_قدر = پورٹ.arst_قدر.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.srst_قدر = پورٹ.srst_قدر.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.addr = پورٹ.sub_addr(it.دوجا);
			پورٹ.wide_log2 = 0;
		}
		پورٹ.transparency_نقاب.clear();
		پورٹ.collision_x_نقاب.clear();
		لكل (خودکار &it2 : new_wr_map)
			پورٹ.transparency_نقاب.ڈالو_پٹھ(orig.transparency_نقاب[it2.پہلا]);
		لكل (خودکار &it2 : new_wr_map)
			پورٹ.collision_x_نقاب.ڈالو_پٹھ(orig.collision_x_نقاب[it2.پہلا]);
		new_rd_ports.ڈالو_پٹھ(پورٹ);
	}
	لكل (خودکار &it : new_wr_map) {
		MemWr &orig = wr_ports[it.پہلا];
		MemWr پورٹ = orig;
		إذا (it.دوجا != 0)
			پورٹ.cell = خالی_سوچک;
		إذا (پورٹ.wide_log2) {
			پورٹ.انکڑے = پورٹ.انکڑے.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.en = پورٹ.en.کڈھو(it.دوجا * چوڑائی, چوڑائی);
			پورٹ.addr = پورٹ.sub_addr(it.دوجا);
			پورٹ.wide_log2 = 0;
		}
		پورٹ.ترجیح_نقاب.clear();
		لكل (خودکار &it2 : new_wr_map)
			پورٹ.ترجیح_نقاب.ڈالو_پٹھ(orig.ترجیح_نقاب[it2.پہلا]);
		new_wr_ports.ڈالو_پٹھ(پورٹ);
	}
	std::وٹاندرا(rd_ports, new_rd_ports);
	std::وٹاندرا(wr_ports, new_wr_ports);
}

فراغ Mem::emulate_ترجیح(صحيح idx1, صحيح idx2, FfInitVals *initvals)
{
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	إذا (!port2.ترجیح_نقاب[idx1])
		أرجع;
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		إذا (rport.transparency_نقاب[idx1] && !(rport.transparency_نقاب[idx2] || rport.collision_x_نقاب[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	صحيح min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	صحيح max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	منطقي wide1 = port1.wide_log2 > port2.wide_log2;
	لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		إذا (wide1)
			addr1 = port1.sub_addr(sub);
		وإلا
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		صحيح ewidth = چوڑائی << min_wide_log2;
		صحيح sub1 = wide1 ? sub : 0;
		صحيح sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ>, Sig_ثنائی_ہندسہ> cache;
		لكل (صحيح pos = 0; pos < ewidth; pos++) {
			Sig_ثنائی_ہندسہ &en1 = port1.en[pos + sub1 * چوڑائی];
			Sig_ثنائی_ہندسہ &en2 = port2.en[pos + sub2 * چوڑائی];
			std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ> کلید(en1, en2);
			إذا (cache.گنتی(کلید)) {
				en1 = cache[کلید];
			} وإلا {
				Sig_ثنائی_ہندسہ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_ثنائی_ہندسہ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[کلید] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ترجیح_نقاب[idx1] = خطأ;
}

فراغ Mem::emulate_transparency(صحيح widx, صحيح ridx, FfInitVals *initvals) {
	خودکار &wport = wr_ports[widx];
	خودکار &rport = rd_ports[ridx];
	log_assert(rport.transparency_نقاب[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	لكل (صحيح i = Get_ناپ(wr_ports) - 1; i > widx; i--) {
		إذا (wr_ports[i].removed)
			استمر;
		إذا (rport.transparency_نقاب[i] && wr_ports[i].ترجیح_نقاب[widx])
			emulate_transparency(i, ridx, initvals);
	}
	صحيح min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	صحيح max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	منطقي wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->جوڑو_Wire(NEW_ID, Get_ناپ(wport.انکڑے));
	module->جوڑو_Dffe(NEW_ID, rport.clk, rport.en, wport.انکڑے, wdata_q, rport.clk_polarity, سچ);
	لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		لكل (صحيح j = min_wide_log2; j < max_wide_log2; j++)
			إذا (wide_write)
				waddr = wport.sub_addr(sub);
			وإلا
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		إذا (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		صحيح pos = 0;
		صحيح ewidth = چوڑائی << min_wide_log2;
		صحيح wsub = wide_write ? sub : 0;
		صحيح rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->جوڑو_Wire(NEW_ID, ewidth);
		طالما (pos < ewidth) {
			صحيح epos = pos;
			طالما (epos < ewidth && wport.en[epos + wsub * چوڑائی] == wport.en[pos + wsub * چوڑائی])
				epos++;
			SigSpec cond;
			إذا (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * چوڑائی], addr_eq);
			وإلا
				cond = wport.en[pos + wsub * چوڑائی];
			SigSpec cond_q = module->جوڑو_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_انکڑے ff(module, initvals, NEW_ID);
			ff.چوڑائی = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = سچ;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			إذا (rport.en != State::S1) {
				ff.has_ce = سچ;
				ff.sig_ce = rport.en;
				ff.pol_ce = سچ;
			}
			إذا (rport.arst != State::S0) {
				ff.has_arst = سچ;
				ff.sig_arst = rport.arst;
				ff.pol_arst = سچ;
				ff.val_arst = State::S0;
			}
			إذا (rport.srst != State::S0) {
				ff.has_srst = سچ;
				ff.sig_srst = rport.srst;
				ff.pol_srst = سچ;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			إذا (!rport.init_قدر.is_fully_undef())
				ff.val_init = State::S0;
			وإلا
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.کڈھو(pos, epos-pos);
			SigSpec other = wdata_q.کڈھو(pos + wsub * چوڑائی, epos-pos);
			SigSpec dest = rport.انکڑے.کڈھو(pos + rsub * چوڑائی, epos-pos);
			module->جوڑو_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.انکڑے.replace(rsub * چوڑائی, rdata_a);
	}
	rport.transparency_نقاب[widx] = خطأ;
	rport.collision_x_نقاب[widx] = سچ;
}

فراغ Mem::prepare_wr_ملاؤ(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	لكل (صحيح i = 0; i < idx1; i++)
		إذا (port2.ترجیح_نقاب[i])
			port1.ترجیح_نقاب[i] = سچ;
	// If port 2 has priority over a port after port 1, emulate it.
	لكل (صحيح i = idx1 + 1; i < idx2; i++)
		إذا (port2.ترجیح_نقاب[i] && !wr_ports[i].removed)
			emulate_ترجیح(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	لكل (صحيح i = idx2 + 1; i < Get_ناپ(wr_ports); i++) {
		خودکار &oport = wr_ports[i];
		إذا (oport.ترجیح_نقاب[idx2])
			oport.ترجیح_نقاب[idx1] = سچ;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		// If collision already undefined with both ports, it's fine.
		إذا (rport.collision_x_نقاب[idx1] && rport.collision_x_نقاب[idx2])
			استمر;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		إذا (rport.collision_x_نقاب[idx1]) {
			rport.collision_x_نقاب[idx1] = خطأ;
			rport.transparency_نقاب[idx1] = rport.transparency_نقاب[idx2];
			استمر;
		}
		إذا (rport.collision_x_نقاب[idx2]) {
			rport.collision_x_نقاب[idx2] = خطأ;
			rport.transparency_نقاب[idx2] = rport.transparency_نقاب[idx1];
			استمر;
		}
		// If transparent with both ports, also fine.
		إذا (rport.transparency_نقاب[idx1] && rport.transparency_نقاب[idx2])
			استمر;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		إذا (rport.transparency_نقاب[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_نقاب[idx1] = خطأ;
			استمر;
		}
		إذا (rport.transparency_نقاب[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_نقاب[idx2] = خطأ;
			استمر;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

فراغ Mem::prepare_rd_ملاؤ(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	خودکار &port1 = rd_ports[idx1];
	خودکار &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	لكل (صحيح i = 0; i < Get_ناپ(wr_ports); i++) {
		إذا (wr_ports[i].removed)
			استمر;
		// Both ports undefined, OK.
		إذا (port1.collision_x_نقاب[i] && port2.collision_x_نقاب[i])
			استمر;
		// Only one port undefined — change its behavior
		// to align with the other port.
		إذا (port1.collision_x_نقاب[i]) {
			port1.collision_x_نقاب[i] = خطأ;
			port1.transparency_نقاب[i] = port2.transparency_نقاب[i];
			استمر;
		}
		إذا (port2.collision_x_نقاب[i]) {
			port2.collision_x_نقاب[i] = خطأ;
			port2.transparency_نقاب[i] = port1.transparency_نقاب[i];
			استمر;
		}
		// Both ports transparent, OK.
		إذا (port1.transparency_نقاب[i] && port2.transparency_نقاب[i])
			استمر;
		// Only one port transparent — emulate transparency
		// on the other.
		إذا (port1.transparency_نقاب[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_نقاب[i] = خطأ;
			استمر;
		}
		إذا (port2.transparency_نقاب[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_نقاب[i] = خطأ;
			استمر;
		}
		// No ports transparent, OK.
	}

}

فراغ Mem::widen_prep(صحيح wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	صحيح نقاب = ((1 << wide_log2) - 1);
	صحيح delta = شروع_وتھ & نقاب;
	شروع_وتھ -= delta;
	ناپ += delta;
	إذا (ناپ & نقاب) {
		ناپ |= نقاب;
		ناپ++;
	}
}

فراغ Mem::widen_wr_پورٹ(صحيح idx, صحيح wide_log2) {
	widen_prep(wide_log2);
	خودکار &پورٹ = wr_ports[idx];
	log_assert(پورٹ.wide_log2 <= wide_log2);
	إذا (پورٹ.wide_log2 < wide_log2) {
		SigSpec new_انکڑے, new_en;
		SigSpec addr_lo = پورٹ.addr.کڈھو(0, wide_log2);
		لكل (صحيح sub = 0; sub < (1 << wide_log2); sub += (1 << پورٹ.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			إذا (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_انکڑے.append(پورٹ.انکڑے);
				new_en.append(پورٹ.en);
			} وإلا إذا (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_انکڑے.append(Const(State::Sx, Get_ناپ(پورٹ.انکڑے)));
				new_en.append(Const(State::S0, Get_ناپ(پورٹ.انکڑے)));
			} وإلا {
				// May or may not write to this subword.
				new_انکڑے.append(پورٹ.انکڑے);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_ناپ(پورٹ.انکڑے)), پورٹ.en, addr_eq);
				new_en.append(en);
			}
		}
		پورٹ.addr.replace(پورٹ.wide_log2, Const(State::S0, wide_log2 - پورٹ.wide_log2));
		پورٹ.انکڑے = new_انکڑے;
		پورٹ.en = new_en;
		پورٹ.wide_log2 = wide_log2;
	}
}

فراغ Mem::emulate_rden(صحيح idx, FfInitVals *initvals) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
	Wire *prev_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
	Wire *sel = module->جوڑو_Wire(NEW_ID);
	Ff_انکڑے ff_sel(module, initvals, NEW_ID);
	Ff_انکڑے ff_انکڑے(module, initvals, NEW_ID);
	ff_sel.چوڑائی = 1;
	ff_sel.has_clk = سچ;
	ff_sel.sig_clk = پورٹ.clk;
	ff_sel.pol_clk = پورٹ.clk_polarity;
	ff_sel.sig_d = پورٹ.en;
	ff_sel.sig_q = sel;
	ff_انکڑے.چوڑائی = Get_ناپ(پورٹ.انکڑے);
	ff_انکڑے.has_clk = سچ;
	ff_انکڑے.sig_clk = پورٹ.clk;
	ff_انکڑے.pol_clk = پورٹ.clk_polarity;
	ff_انکڑے.sig_d = پورٹ.انکڑے;
	ff_انکڑے.sig_q = prev_انکڑے;
	إذا (!پورٹ.init_قدر.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_انکڑے.val_init = پورٹ.init_قدر;
		پورٹ.init_قدر = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));
	} وإلا {
		ff_sel.val_init = State::Sx;
		ff_انکڑے.val_init = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));
	}
	إذا (پورٹ.arst != State::S0) {
		ff_sel.has_arst = سچ;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = پورٹ.arst;
		ff_sel.pol_arst = سچ;
		ff_انکڑے.has_arst = سچ;
		ff_انکڑے.val_arst = پورٹ.arst_قدر;
		ff_انکڑے.sig_arst = پورٹ.arst;
		ff_انکڑے.pol_arst = سچ;
		پورٹ.arst = State::S0;
	}
	إذا (پورٹ.srst != State::S0) {
		log_assert(!پورٹ.ce_over_srst);
		ff_sel.has_srst = سچ;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = پورٹ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.ce_over_srst = خطأ;
		ff_انکڑے.has_srst = سچ;
		ff_انکڑے.val_srst = پورٹ.srst_قدر;
		ff_انکڑے.sig_srst = پورٹ.srst;
		ff_انکڑے.pol_srst = سچ;
		ff_انکڑے.ce_over_srst = خطأ;
		پورٹ.srst = State::S0;
	}
	ff_sel.emit();
	ff_انکڑے.emit();
	module->جوڑو_Mux(NEW_ID, prev_انکڑے, new_انکڑے, sel, پورٹ.انکڑے);
	پورٹ.انکڑے = new_انکڑے;
	پورٹ.en = State::S1;
}

فراغ Mem::emulate_reset(صحيح idx, منطقي emu_init, منطقي emu_arst, منطقي emu_srst, FfInitVals *initvals) {
	خودکار &پورٹ = rd_ports[idx];
	إذا (emu_init && !پورٹ.init_قدر.is_fully_undef()) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_انکڑے ff_sel(module, initvals, NEW_ID);
		Wire *new_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		إذا (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		إذا (پورٹ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٹ.arst;
			ff_sel.pol_arst = سچ;
			إذا (emu_arst && پورٹ.arst_قدر == پورٹ.init_قدر) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				پورٹ.arst = State::S0;
			} وإلا {
				ff_sel.val_arst = State::S1;
			}
		}
		إذا (پورٹ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٹ.srst;
			ff_sel.pol_srst = سچ;
			إذا (emu_srst && پورٹ.srst_قدر == پورٹ.init_قدر) {
				ff_sel.val_srst = State::S0;
				پورٹ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.init_قدر, new_انکڑے, sel, پورٹ.انکڑے);
		پورٹ.انکڑے = new_انکڑے;
		پورٹ.init_قدر = Const(State::Sx, Get_ناپ(پورٹ.انکڑے));
	}
	إذا (emu_arst && پورٹ.arst != State::S0) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_انکڑے ff_sel(module, initvals, NEW_ID);
		Wire *new_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورٹ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		ff_sel.has_arst = سچ;
		ff_sel.sig_arst = پورٹ.arst;
		ff_sel.pol_arst = سچ;
		ff_sel.val_arst = State::S0;
		إذا (پورٹ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٹ.srst;
			ff_sel.pol_srst = سچ;
			إذا (emu_srst && پورٹ.srst_قدر == پورٹ.arst_قدر) {
				ff_sel.val_srst = State::S0;
				پورٹ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.arst_قدر, new_انکڑے, sel, پورٹ.انکڑے);
		پورٹ.انکڑے = new_انکڑے;
		پورٹ.arst = State::S0;
	}
	إذا (emu_srst && پورٹ.srst != State::S0) {
		Wire *sel = module->جوڑو_Wire(NEW_ID);
		Ff_انکڑے ff_sel(module, initvals, NEW_ID);
		Wire *new_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
		ff_sel.چوڑائی = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٹ.clk;
		ff_sel.pol_clk = پورٹ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورٹ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورٹ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٹ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٹ.ce_over_srst;
		}
		ff_sel.has_srst = سچ;
		ff_sel.sig_srst = پورٹ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.val_srst = State::S0;
		إذا (پورٹ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٹ.arst;
			ff_sel.pol_arst = سچ;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->جوڑو_Mux(NEW_ID, پورٹ.srst_قدر, new_انکڑے, sel, پورٹ.انکڑے);
		پورٹ.انکڑے = new_انکڑے;
		پورٹ.srst = State::S0;
	}
}

فراغ Mem::emulate_rd_ce_over_srst(صحيح idx) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	إذا (پورٹ.en == State::S1 || پورٹ.srst == State::S0 || !پورٹ.ce_over_srst) {
		پورٹ.ce_over_srst = خطأ;
		أرجع;
	}
	پورٹ.ce_over_srst = خطأ;
	پورٹ.srst = module->And(NEW_ID, پورٹ.en, پورٹ.srst);
}

فراغ Mem::emulate_rd_srst_over_ce(صحيح idx) {
	خودکار &پورٹ = rd_ports[idx];
	log_assert(پورٹ.clk_enable);
	إذا (پورٹ.en == State::S1 || پورٹ.srst == State::S0 || پورٹ.ce_over_srst) {
		پورٹ.ce_over_srst = سچ;
		أرجع;
	}
	پورٹ.ce_over_srst = سچ;
	پورٹ.en = module->Or(NEW_ID, پورٹ.en, پورٹ.srst);
}

منطقي Mem::emulate_read_پہلا_ok() {
	إذا (wr_ports.خالی_تھاواں())
		أرجع خطأ;
	SigSpec clk = wr_ports[0].clk;
	منطقي clk_polarity = wr_ports[0].clk_polarity;
	لكل (خودکار &پورٹ: wr_ports) {
		إذا (!پورٹ.clk_enable)
			أرجع خطأ;
		إذا (پورٹ.clk != clk)
			أرجع خطأ;
		إذا (پورٹ.clk_polarity != clk_polarity)
			أرجع خطأ;
	}
	منطقي لبھیا_read_پہلا = خطأ;
	لكل (خودکار &پورٹ: rd_ports) {
		إذا (!پورٹ.clk_enable)
			أرجع خطأ;
		إذا (پورٹ.clk != clk)
			أرجع خطأ;
		إذا (پورٹ.clk_polarity != clk_polarity)
			أرجع خطأ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		لكل (صحيح j = 0; j < Get_ناپ(wr_ports); j++)
			إذا (!پورٹ.transparency_نقاب[j] && !پورٹ.collision_x_نقاب[j])
				لبھیا_read_پہلا = سچ;
	}
	أرجع لبھیا_read_پہلا;
}

فراغ Mem::emulate_read_پہلا(FfInitVals *initvals) {
	log_assert(emulate_read_پہلا_ok());
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_ناپ(wr_ports); j++)
			إذا (rd_ports[i].transparency_نقاب[j])
				emulate_transparency(j, i, initvals);
	لكل (صحيح i = 0; i < Get_ناپ(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_ناپ(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_نقاب[j]);
			rd_ports[i].collision_x_نقاب[j] = خطأ;
			rd_ports[i].transparency_نقاب[j] = سچ;
		}
	لكل (خودکار &پورٹ: wr_ports) {
		Wire *new_انکڑے = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.انکڑے));
		Wire *new_addr = module->جوڑو_Wire(NEW_ID, Get_ناپ(پورٹ.addr));
		خودکار compressed = پورٹ.compress_en();
		Wire *new_en = module->جوڑو_Wire(NEW_ID, Get_ناپ(compressed.پہلا));
		Ff_انکڑے ff_انکڑے(module, initvals, NEW_ID);
		Ff_انکڑے ff_addr(module, initvals, NEW_ID);
		Ff_انکڑے ff_en(module, initvals, NEW_ID);
		ff_انکڑے.چوڑائی = Get_ناپ(پورٹ.انکڑے);
		ff_انکڑے.has_clk = سچ;
		ff_انکڑے.sig_clk = پورٹ.clk;
		ff_انکڑے.pol_clk = پورٹ.clk_polarity;
		ff_انکڑے.sig_d = پورٹ.انکڑے;
		ff_انکڑے.sig_q = new_انکڑے;;
		ff_انکڑے.val_init = Const(State::Sx, ff_انکڑے.چوڑائی);
		ff_انکڑے.emit();
		ff_addr.چوڑائی = Get_ناپ(پورٹ.addr);
		ff_addr.has_clk = سچ;
		ff_addr.sig_clk = پورٹ.clk;
		ff_addr.pol_clk = پورٹ.clk_polarity;
		ff_addr.sig_d = پورٹ.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.چوڑائی);
		ff_addr.emit();
		ff_en.چوڑائی = Get_ناپ(compressed.پہلا);
		ff_en.has_clk = سچ;
		ff_en.sig_clk = پورٹ.clk;
		ff_en.pol_clk = پورٹ.clk_polarity;
		ff_en.sig_d = compressed.پہلا;
		ff_en.sig_q = new_en;;
		إذا (inits.خالی_تھاواں())
			ff_en.val_init = Const(State::Sx, ff_en.چوڑائی);
		وإلا
			ff_en.val_init = Const(State::S0, ff_en.چوڑائی);
		ff_en.emit();
		پورٹ.انکڑے = new_انکڑے;
		پورٹ.addr = new_addr;
		پورٹ.en = پورٹ.decompress_en(compressed.دوجا, new_en);
	}
}

std::pair<SigSpec, std::سمتیہ<صحيح>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::سمتیہ<صحيح> swizzle;
	Sig_ثنائی_ہندسہ prev_ثنائی_ہندسہ = en[0];
	صحيح idx = 0;
	لكل (خودکار &ثنائی_ہندسہ: en) {
		إذا (ثنائی_ہندسہ != prev_ثنائی_ہندسہ) {
			sig.append(ثنائی_ہندسہ);
			prev_ثنائی_ہندسہ = ثنائی_ہندسہ;
			idx++;
		}
		swizzle.ڈالو_پٹھ(idx);
	}
	log_assert(idx + 1 == Get_ناپ(sig));
	أرجع {sig, swizzle};
}

SigSpec MemWr::decompress_en(ثابت std::سمتیہ<صحيح> &swizzle, SigSpec sig) {
	SigSpec res;
	لكل (صحيح i: swizzle)
		res.append(sig[i]);
	أرجع res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->ناپ), mem->چوڑائی)
{
	لكل(ثابت خودکار &init : mem->inits) {
		إذا(init.en.is_fully_zero()) استمر;
		log_assert(init.en.ناپ() == انکڑے_چوڑائی_2);
		إذا(init.en.is_fully_ones())
			داخل_کرو_concatenated(init.addr.as_int(), init.انکڑے);
		وإلا {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t الفاظ_2 = init.انکڑے.ناپ() / انکڑے_چوڑائی_2;
			RTLIL::Const انکڑے = init.انکڑے;
			log_assert(انکڑے.ناپ() % انکڑے_چوڑائی_2 == 0);
			لكل(addr_t i = 0; i < الفاظ_2; i++) {
				RTLIL::Const پچھلا = (*هذا)[addr + i];
				لكل(صحيح j = 0; j < انکڑے_چوڑائی_2; j++)
					إذا(init.en[j] != State::S1)
						انکڑے.سیٹ(انکڑے_چوڑائی_2 * i + j, پچھلا[j]);
			}
			داخل_کرو_concatenated(init.addr.as_int(), انکڑے);
		}
	}
}

MemContents::iterator & MemContents::iterator::عامل++() {
	خودکار it = حافظہ_2->اقدار_2.اتلا_bound(_addr);
	إذا(it == حافظہ_2->اقدار_2.سرا()) {
		حافظہ_2 = خالی_سوچک;
		_addr = ~(addr_t) 0;
	} وإلا
		_addr = it->پہلا;
	أرجع *هذا;
}

فراغ MemContents::check() {
	log_assert(addr_چوڑائی_2 > 0 && addr_چوڑائی_2 < (صحيح)حجم(addr_t) * 8);
	log_assert(انکڑے_چوڑائی_2 > 0);
	log_assert(default_قدر.ناپ() == انکڑے_چوڑائی_2);
	إذا(اقدار_2.خالی_تھاواں()) أرجع;
	خودکار it = اقدار_2.begin();
	لكل(;;) {
		log_assert(!it->دوجا.خالی_تھاواں());
		log_assert(it->دوجا.ناپ() % انکڑے_چوڑائی_2 == 0);
		خودکار end1 = range_سرا(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_چوڑائی_2));
		log_assert(end1 <= (addr_t)(1<<addr_چوڑائی_2));
		إذا(++it == اقدار_2.سرا())
			اكسر;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

منطقي MemContents::range_شامل_اے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	أرجع it != اقدار_2.سرا() && addr - _range_begin(it) < range_ناپ(it);
}


منطقي MemContents::range_شامل_اے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t سرا_addr) ثابت {
	// note that we assume begin_addr <= end_addr
	أرجع it != اقدار_2.سرا() && _range_begin(it) <= begin_addr && سرا_addr - _range_begin(it) <= range_ناپ(it);
}

منطقي MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t سرا_addr) ثابت {
	إذا(it == اقدار_2.سرا() || begin_addr >= سرا_addr)
		أرجع خطأ;
	خودکار top1 = range_سرا(it) - 1;
	خودکار top2 = سرا_addr - 1;
	أرجع !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ثابت {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_چوڑائی_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	خودکار it = const_cast<std::map<addr_t, RTLIL::Const> &>(اقدار_2).اتلا_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	إذا(it == اقدار_2.begin())
		أرجع it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	خودکار it_prev = std::اگلا(it, -1);
	إذا(range_شامل_اے(it_prev, addr))
		أرجع it_prev;
	وإلا
		أرجع it;
}

RTLIL::Const MemContents::عامل[](addr_t addr) ثابت {
	خودکار it = _range_at(addr);
	إذا(range_شامل_اے(it, addr))
		أرجع it->دوجا.کڈھو(range_وتھ(it, addr), انکڑے_چوڑائی_2);
	وإلا
		أرجع default_قدر;
}

MemContents::addr_t MemContents::گنتی_range(addr_t begin_addr, addr_t سرا_addr) ثابت {
	addr_t گنتی = 0;
	لكل(خودکار it = _range_at(begin_addr); _range_overlaps(it, begin_addr, سرا_addr); it++) {
		خودکار پہلا = std::max(_range_begin(it), begin_addr);
		خودکار آخری = std::min(range_سرا(it), سرا_addr);
		گنتی += آخری - پہلا;
	}
	أرجع گنتی;
}

فراغ MemContents::clear_range(addr_t begin_addr, addr_t سرا_addr) {
	إذا(begin_addr >= سرا_addr) أرجع;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	خودکار begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	خودکار سرا_it = اقدار_2.اتلا_bound(سرا_addr - 1);
	إذا(begin_it == سرا_it)
		أرجع; // nothing to do
	// the last iterator affected is one before the first one not affected
	خودکار آخری_it = std::اگلا(سرا_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	إذا(range_شامل_اے(آخری_it, سرا_addr - 1)) {
		خودکار new_begin = سرا_addr;
		خودکار سرا = range_سرا(آخری_it);
		// if there is data past the end address, preserve it by creating a new range
		إذا(new_begin != سرا)
			سرا_it = اقدار_2.emplace_hint(آخری_it, new_begin, آخری_it->دوجا.کڈھو(range_وتھ(آخری_it, new_begin), (range_سرا(آخری_it) - new_begin) * انکڑے_چوڑائی_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	إذا(range_شامل_اے(begin_it, begin_addr)) {
		خودکار new_سرا = begin_addr;
		// if there is data before the start address, truncate but don't delete
		إذا(new_سرا != begin_it->پہلا) {
			begin_it->دوجا.extu(range_وتھ(begin_it, new_سرا));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	اقدار_2.erase(begin_it, سرا_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t سرا_addr) {
	إذا(begin_addr >= سرا_addr)
		أرجع اقدار_2.سرا(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	خودکار ہیٹھلا_it = begin_addr == 0 ? اقدار_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	إذا (range_شامل_اے(ہیٹھلا_it, begin_addr, سرا_addr))
		أرجع ہیٹھلا_it;
	// find the first range containing any addr >= end_addr
	خودکار اتلا_it = _range_at(سرا_addr);
	// check if either of the two ranges we just found touch our range
	منطقي ہیٹھلا_touch = begin_addr > 0 && range_شامل_اے(ہیٹھلا_it, begin_addr - 1);
	منطقي اتلا_touch = range_شامل_اے(اتلا_it, سرا_addr);
	إذا (ہیٹھلا_touch && اتلا_touch) {
		log_assert (ہیٹھلا_it != اتلا_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		خودکار اتلا_سرا = range_سرا(اتلا_it);
		// make range bigger (maybe reserve here instead of resize?)
		ہیٹھلا_it->دوجا.resize(range_وتھ(ہیٹھلا_it, اتلا_سرا), State::Sx);
		// copy only the data beyond our range
		std::copy(range_انکڑے(اتلا_it, سرا_addr), range_انکڑے(اتلا_it, اتلا_سرا), range_انکڑے(ہیٹھلا_it, سرا_addr));
		// keep lower_it, but delete upper_it
		اقدار_2.erase(std::اگلا(ہیٹھلا_it), std::اگلا(اتلا_it));
		أرجع ہیٹھلا_it;
	} وإلا إذا (ہیٹھلا_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		ہیٹھلا_it->دوجا.resize(range_وتھ(ہیٹھلا_it, سرا_addr), State::Sx);
		// keep lower_it and upper_it
		اقدار_2.erase(std::اگلا(ہیٹھلا_it), اتلا_it);
		أرجع ہیٹھلا_it;
	} وإلا إذا (اتلا_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const انکڑے(State::Sx, (_range_begin(اتلا_it) - begin_addr) * انکڑے_چوڑائی_2);
		انکڑے.append(std::move(اتلا_it->دوجا));
		// delete lower_it and upper_it, then reinsert
		اقدار_2.erase(ہیٹھلا_it, std::اگلا(اتلا_it));
		أرجع اقدار_2.emplace(begin_addr, std::move(انکڑے)).پہلا;
	} وإلا {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		اقدار_2.erase(ہیٹھلا_it, اتلا_it);
		أرجع اقدار_2.emplace(begin_addr, RTLIL::Const(State::Sx, (سرا_addr - begin_addr) * انکڑے_چوڑائی_2)).پہلا;
	}
}

فراغ MemContents::داخل_کرو_concatenated(addr_t addr, RTLIL::Const ثابت &اقدار) {
	addr_t الفاظ_2 = (اقدار.ناپ() + انکڑے_چوڑائی_2 - 1) / انکڑے_چوڑائی_2;
	log_assert(addr < (addr_t)(1<<addr_چوڑائی_2));
	log_assert(الفاظ_2 <= (addr_t)(1<<addr_چوڑائی_2) - addr);
	خودکار it = _reserve_range(addr, addr + الفاظ_2);
	خودکار منزل_begin = range_انکڑے(it, addr);
	std::copy(اقدار.begin(), اقدار.سرا(), منزل_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(منزل_begin + اقدار.ناپ(), منزل_begin + الفاظ_2 * انکڑے_چوڑائی_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &لفظ) {
	خودکار from_سرا = لفظ.ناپ() <= انکڑے_چوڑائی_2 ? لفظ.سرا() : لفظ.begin() + انکڑے_چوڑائی_2;
	خودکار منزل_سرا = std::copy(لفظ.begin(), from_سرا, it);
	خودکار it_اگلا = std::اگلا(it, انکڑے_چوڑائی_2);
	std::fill(منزل_سرا, it_اگلا, State::S0);
	أرجع it_اگلا;
}
