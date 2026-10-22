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
		cell = خالي_اشارو;
	}
	إذا (mem) {
		module->memories.erase(mem->name);
		ميٽيو mem;
		mem = خالي_اشارو;
	}
	لكل (خودڪار &پورٽ : rd_ports) {
		إذا (پورٽ.cell) {
			module->remove(پورٽ.cell);
			پورٽ.cell = خالي_اشارو;
		}
	}
	لكل (خودڪار &پورٽ : wr_ports) {
		إذا (پورٽ.cell) {
			module->remove(پورٽ.cell);
			پورٽ.cell = خالي_اشارو;
		}
	}
	لكل (خودڪار &init : inits) {
		إذا (init.cell) {
			module->remove(init.cell);
			init.cell = خالي_اشارو;
		}
	}
}

فراغ Mem::emit() {
	check();
	std::ويڪٽر<صحيح> rd_کاٻو;
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++) {
		خودڪار &پورٽ = rd_ports[i];
		إذا (پورٽ.removed) {
			إذا (پورٽ.cell) {
				module->remove(پورٽ.cell);
			}
		} وإلا {
			rd_کاٻو.ڌڪو_پٺ(i);
		}
	}
	std::ويڪٽر<صحيح> wr_کاٻو;
	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
		خودڪار &پورٽ = wr_ports[i];
		إذا (پورٽ.removed) {
			إذا (پورٽ.cell) {
				module->remove(پورٽ.cell);
			}
		} وإلا {
			wr_کاٻو.ڌڪو_پٺ(i);
		}
	}
	std::ويڪٽر<صحيح> init_کاٻو;
	لكل (صحيح i = 0; i < Get_سائيز(inits); i++) {
		خودڪار &init = inits[i];
		إذا (init.removed) {
			إذا (init.cell) {
				module->remove(init.cell);
			}
		} وإلا {
			init_کاٻو.ڌڪو_پٺ(i);
		}
	}
	لكل (صحيح i = 0; i < Get_سائيز(rd_کاٻو); i++)
		إذا (i != rd_کاٻو[i])
			std::مٽا_سٽا(rd_ports[i], rd_ports[rd_کاٻو[i]]);
	rd_ports.resize(Get_سائيز(rd_کاٻو));
	لكل (صحيح i = 0; i < Get_سائيز(wr_کاٻو); i++)
		إذا (i != wr_کاٻو[i])
			std::مٽا_سٽا(wr_ports[i], wr_ports[wr_کاٻو[i]]);
	wr_ports.resize(Get_سائيز(wr_کاٻو));
	لكل (صحيح i = 0; i < Get_سائيز(init_کاٻو); i++)
		إذا (i != init_کاٻو[i])
			std::مٽا_سٽا(inits[i], inits[init_کاٻو[i]]);
	inits.resize(Get_سائيز(init_کاٻو));

	لكل (خودڪار &پورٽ : rd_ports) {
		لكل (صحيح i = 0; i < Get_سائيز(wr_کاٻو); i++) {
			پورٽ.transparency_نقاب[i] = پورٽ.transparency_نقاب[wr_کاٻو[i]];
			پورٽ.collision_x_نقاب[i] = پورٽ.collision_x_نقاب[wr_کاٻو[i]];
		}
		پورٽ.transparency_نقاب.resize(Get_سائيز(wr_کاٻو));
		پورٽ.collision_x_نقاب.resize(Get_سائيز(wr_کاٻو));
	}
	لكل (خودڪار &پورٽ : wr_ports) {
		لكل (صحيح i = 0; i < Get_سائيز(wr_کاٻو); i++)
			پورٽ.ترجيح_نقاب[i] = پورٽ.ترجيح_نقاب[wr_کاٻو[i]];
		پورٽ.ترجيح_نقاب.resize(Get_سائيز(wr_کاٻو));
	}

	إذا (packed) {
		إذا (mem) {
			module->memories.erase(mem->name);
			ميٽيو mem;
			mem = خالي_اشارو;
		}
		إذا (!cell) {
			إذا (memid.خالي_جايون())
				memid = PooledName(module->design, module->twines().جوڙيو(NEW_ID));
			cell = module->جوڙيو_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->خاصيتون = خاصيتون;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::ويڪر_2] = Const(ويڪر);
		cell->parameters[ID::وٿي_2] = Const(شروعات_وٿي);
		cell->parameters[ID::سائيز_2] = Const(سائيز);
		SigSpec rd_clk, rd_en, rd_addr, rd_معلومات;
		SigSpec wr_clk, wr_en, wr_addr, wr_معلومات;
		SigSpec rd_arst, rd_srst;
		صحيح abits = 0;
		لكل (خودڪار &پورٽ : rd_ports)
			abits = std::max(abits, Get_سائيز(پورٽ.addr));
		لكل (خودڪار &پورٽ : wr_ports)
			abits = std::max(abits, Get_سائيز(پورٽ.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::ويڪٽر<صحيح> wr_پورٽ_xlat;
		لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++)
			لكل (صحيح j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_پورٽ_xlat.ڌڪو_پٺ(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_نقاب_builder;
		Const::Builder rd_collision_x_نقاب_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_قدر_builder;
		Const::Builder rd_srst_قدر_builder;
		Const::Builder rd_init_قدر_builder;
		لكل (خودڪار &پورٽ : rd_ports) {
			لكل (خودڪار attr: پورٽ.خاصيتون)
				إذا (!cell->has_attribute(attr.پهريون))
					cell->خاصيتون.وجهو(attr);
			إذا (پورٽ.cell) {
				module->remove(پورٽ.cell);
				پورٽ.cell = خالي_اشارو;
			}
			لكل (صحيح sub = 0; sub < (1 << پورٽ.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ڌڪو_پٺ(State(sub != 0));
				rd_clk_enable_builder.ڌڪو_پٺ(State(پورٽ.clk_enable));
				rd_clk_polarity_builder.ڌڪو_پٺ(State(پورٽ.clk_polarity));
				rd_ce_over_srst_builder.ڌڪو_پٺ(State(پورٽ.ce_over_srst));
				rd_clk.append(پورٽ.clk);
				rd_arst.append(پورٽ.arst);
				rd_srst.append(پورٽ.srst);
				rd_en.append(پورٽ.en);
				SigSpec addr = پورٽ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				rd_addr.append(addr);
				log_assert(Get_سائيز(addr) == abits);
				لكل (خودڪار idx : wr_پورٽ_xlat) {
					rd_transparency_نقاب_builder.ڌڪو_پٺ(State(منطقي(پورٽ.transparency_نقاب[idx])));
					rd_collision_x_نقاب_builder.ڌڪو_پٺ(State(منطقي(پورٽ.collision_x_نقاب[idx])));
				}
			}
			rd_معلومات.append(پورٽ.معلومات);
			لكل (خودڪار ٻٽو_انگ : پورٽ.arst_قدر)
				rd_arst_قدر_builder.ڌڪو_پٺ(ٻٽو_انگ);
			لكل (خودڪار ٻٽو_انگ : پورٽ.srst_قدر)
				rd_srst_قدر_builder.ڌڪو_پٺ(ٻٽو_انگ);
			لكل (خودڪار ٻٽو_انگ : پورٽ.init_قدر)
				rd_init_قدر_builder.ڌڪو_پٺ(ٻٽو_انگ);
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
		إذا (rd_ports.خالي_جايون()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_قدر = State::S0;
			rd_srst_قدر = State::S0;
			rd_init_قدر = State::S0;
		}
		إذا (rd_ports.خالي_جايون() || wr_ports.خالي_جايون()) {
			rd_transparency_نقاب = State::S0;
			rd_collision_x_نقاب = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_سائيز(rd_clk));
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
		cell->سيٽ_پورٽ(ID::RD_CLK, rd_clk);
		cell->سيٽ_پورٽ(ID::RD_EN, rd_en);
		cell->سيٽ_پورٽ(ID::RD_ARST, rd_arst);
		cell->سيٽ_پورٽ(ID::RD_SRST, rd_srst);
		cell->سيٽ_پورٽ(ID::RD_ADDR, rd_addr);
		cell->سيٽ_پورٽ(ID::RD_معلومات, rd_معلومات);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ترجيح_نقاب_builder;
		لكل (خودڪار &پورٽ : wr_ports) {
			لكل (خودڪار attr: پورٽ.خاصيتون)
				إذا (!cell->has_attribute(attr.پهريون))
					cell->خاصيتون.وجهو(attr);
			إذا (پورٽ.cell) {
				module->remove(پورٽ.cell);
				پورٽ.cell = خالي_اشارو;
			}
			لكل (صحيح sub = 0; sub < (1 << پورٽ.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ڌڪو_پٺ(State(sub != 0));
				wr_clk_enable_builder.ڌڪو_پٺ(State(پورٽ.clk_enable));
				wr_clk_polarity_builder.ڌڪو_پٺ(State(پورٽ.clk_polarity));
				wr_clk.append(پورٽ.clk);
				لكل (خودڪار idx : wr_پورٽ_xlat)
					wr_ترجيح_نقاب_builder.ڌڪو_پٺ(State(منطقي(پورٽ.ترجيح_نقاب[idx])));
				SigSpec addr = پورٽ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				wr_addr.append(addr);
				log_assert(Get_سائيز(addr) == abits);
			}
			wr_en.append(پورٽ.en);
			wr_معلومات.append(پورٽ.معلومات);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ترجيح_نقاب = wr_ترجيح_نقاب_builder.build();
		إذا (wr_ports.خالي_جايون()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ترجيح_نقاب = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_سائيز(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ترجيح_نقاب] = wr_ترجيح_نقاب;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->سيٽ_پورٽ(ID::WR_CLK, wr_clk);
		cell->سيٽ_پورٽ(ID::WR_EN, wr_en);
		cell->سيٽ_پورٽ(ID::WR_ADDR, wr_addr);
		cell->سيٽ_پورٽ(ID::WR_معلومات, wr_معلومات);
		لكل (خودڪار &init : inits) {
			لكل (خودڪار attr: init.خاصيتون)
				إذا (!cell->has_attribute(attr.پهريون))
					cell->خاصيتون.وجهو(attr);
			إذا (init.cell) {
				module->remove(init.cell);
				init.cell = خالي_اشارو;
			}
		}
		cell->parameters[ID::INIT] = get_init_معلومات();
	} وإلا {
		إذا (cell) {
			module->remove(cell);
			cell = خالي_اشارو;
		}
		إذا (!mem) {
			إذا (memid.خالي_جايون())
				memid = PooledName(module->design, module->twines().جوڙيو(NEW_ID));
			mem = module->جوڙيو_يادگيري(memid);
		}
		mem->ويڪر = ويڪر;
		mem->شروعات_وٿي = شروعات_وٿي;
		mem->سائيز = سائيز;
		mem->خاصيتون = خاصيتون;
		لكل (خودڪار &پورٽ : rd_ports) {
			إذا (!پورٽ.cell)
				پورٽ.cell = module->جوڙيو_Cell(NEW_ID, ID($memrd_v2));
			پورٽ.cell->type = ID($memrd_v2);
			پورٽ.cell->خاصيتون = پورٽ.خاصيتون;
			پورٽ.cell->parameters[ID::MEMID] = memid.str();
			پورٽ.cell->parameters[ID::ABITS] = Get_سائيز(پورٽ.addr);
			پورٽ.cell->parameters[ID::ويڪر_2] = ويڪر << پورٽ.wide_log2;
			پورٽ.cell->parameters[ID::CLK_ENABLE] = پورٽ.clk_enable;
			پورٽ.cell->parameters[ID::CLK_POLARITY] = پورٽ.clk_polarity;
			پورٽ.cell->parameters[ID::CE_OVER_SRST] = پورٽ.ce_over_srst;
			پورٽ.cell->parameters[ID::ARST_قدر] = پورٽ.arst_قدر;
			پورٽ.cell->parameters[ID::SRST_قدر] = پورٽ.srst_قدر;
			پورٽ.cell->parameters[ID::INIT_قدر] = پورٽ.init_قدر;
			پورٽ.cell->parameters[ID::TRANSPARENCY_نقاب] = پورٽ.transparency_نقاب;
			پورٽ.cell->parameters[ID::COLLISION_X_نقاب] = پورٽ.collision_x_نقاب;
			پورٽ.cell->parameters.erase(ID::TRANSPARENT);
			پورٽ.cell->سيٽ_پورٽ(ID::CLK, پورٽ.clk);
			پورٽ.cell->سيٽ_پورٽ(ID::EN, پورٽ.en);
			پورٽ.cell->سيٽ_پورٽ(ID::ARST, پورٽ.arst);
			پورٽ.cell->سيٽ_پورٽ(ID::SRST, پورٽ.srst);
			پورٽ.cell->سيٽ_پورٽ(ID::ADDR, پورٽ.addr);
			پورٽ.cell->سيٽ_پورٽ(ID::معلومات_2, پورٽ.معلومات);
		}
		صحيح idx = 0;
		لكل (خودڪار &پورٽ : wr_ports) {
			إذا (!پورٽ.cell)
				پورٽ.cell = module->جوڙيو_Cell(NEW_ID, ID($memwr_v2));
			پورٽ.cell->type = ID($memwr_v2);
			پورٽ.cell->خاصيتون = پورٽ.خاصيتون;
			إذا (پورٽ.cell->parameters.ڳڻپ(ID::ترجيح))
				پورٽ.cell->parameters.erase(ID::ترجيح);
			پورٽ.cell->parameters[ID::MEMID] = memid.str();
			پورٽ.cell->parameters[ID::ABITS] = Get_سائيز(پورٽ.addr);
			پورٽ.cell->parameters[ID::ويڪر_2] = ويڪر << پورٽ.wide_log2;
			پورٽ.cell->parameters[ID::CLK_ENABLE] = پورٽ.clk_enable;
			پورٽ.cell->parameters[ID::CLK_POLARITY] = پورٽ.clk_polarity;
			پورٽ.cell->parameters[ID::PORTID] = idx++;
			پورٽ.cell->parameters[ID::ترجيح_نقاب_2] = پورٽ.ترجيح_نقاب;
			پورٽ.cell->سيٽ_پورٽ(ID::CLK, پورٽ.clk);
			پورٽ.cell->سيٽ_پورٽ(ID::EN, پورٽ.en);
			پورٽ.cell->سيٽ_پورٽ(ID::ADDR, پورٽ.addr);
			پورٽ.cell->سيٽ_پورٽ(ID::معلومات_2, پورٽ.معلومات);
		}
		idx = 0;
		لكل (خودڪار &init : inits) {
			منطقي v2 = !init.en.is_fully_ones();
			إذا (!init.cell)
				init.cell = module->جوڙيو_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			وإلا {
				إذا (!v2)
					init.cell->unset_پورٽ(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->خاصيتون = init.خاصيتون;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_سائيز(init.addr);
			init.cell->parameters[ID::ويڪر_2] = ويڪر;
			init.cell->parameters[ID::لفظ_2] = Get_سائيز(init.معلومات) / ويڪر;
			init.cell->parameters[ID::ترجيح] = idx++;
			init.cell->سيٽ_پورٽ(ID::ADDR, init.addr);
			init.cell->سيٽ_پورٽ(ID::معلومات_2, init.معلومات);
			إذا (v2)
				init.cell->سيٽ_پورٽ(ID::EN, init.en);
		}
	}
}

فراغ Mem::clear_inits() {
	لكل (خودڪار &init : inits)
		init.removed = سچ;
}

فراغ Mem::coalesce_inits() {
	// start address -> end address
	std::map<صحيح, صحيح> chunks;
	// Figure out chunk boundaries.
	لكل (خودڪار &init : inits) {
		إذا (init.removed)
			استمر;
		منطقي valid = خطأ;
		لكل (خودڪار ٻٽو_انگ : init.en)
			إذا (ٻٽو_انگ == State::S1)
				valid = سچ;
		إذا (!valid) {
			init.removed = سچ;
			استمر;
		}
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_سائيز(init.معلومات) / ويڪر;
		خودڪار it_e = chunks.مٿانهون_bound(addr_e);
		خودڪار it = it_e;
		طالما (it != chunks.begin()) {
			--it;
			إذا (it->ٻيو < addr) {
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
			إذا (it->پهريون < addr)
				addr = it->پهريون;
			خودڪار it_آخري = it_e;
			it_آخري--;
			إذا (it_آخري->ٻيو > addr_e)
				addr_e = it_آخري->ٻيو;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<صحيح, std::ويڪٽر<صحيح>> inits_by_chunk;
	لكل (صحيح i = 0; i < Get_سائيز(inits); i++) {
		خودڪار &init = inits[i];
		إذا (init.removed)
			استمر;
		خودڪار it = chunks.مٿانهون_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->پهريون].ڌڪو_پٺ(i);
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_سائيز(init.معلومات) / ويڪر;
		log_assert(addr >= it->پهريون && addr_e <= it->ٻيو);
	}
	// Process each chunk.
	لكل (خودڪار &it : inits_by_chunk) {
		صحيح caddr = it.پهريون;
		صحيح caddr_e = chunks[caddr];
		خودڪار &chunk_inits = it.ٻيو;
		إذا (Get_سائيز(chunk_inits) == 1) {
			خودڪار &init = inits[chunk_inits[0]];
			إذا (!init.en.is_fully_ones()) {
				لكل (صحيح i = 0; i < Get_سائيز(init.معلومات); i++)
					إذا (init.en[i % ويڪر] != State::S1)
						init.معلومات.سيٽ(i, State::Sx);
				init.en = Const(State::S1, ويڪر);
			}
			استمر;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * ويڪر);
		لكل (صحيح idx : chunk_inits) {
			خودڪار &init = inits[idx];
			صحيح وٿي = (init.addr.as_int() - caddr) * ويڪر;
			log_assert(وٿي >= 0);
			log_assert(وٿي + Get_سائيز(init.معلومات) <= Get_سائيز(cdata));
			لكل (صحيح i = 0; i < Get_سائيز(init.معلومات); i++)
				إذا (init.en[i % ويڪر] == State::S1)
					cdata.سيٽ(i+وٿي, init.معلومات[i]);
			init.removed = سچ;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.معلومات = cdata;
		new_init.en = Const(State::S1, ويڪر);
		inits.ڌڪو_پٺ(new_init);
	}
}

Const Mem::get_init_معلومات() ثابت {
	Const init_معلومات(State::Sx, ويڪر * سائيز);
	لكل (خودڪار &init : inits) {
		إذا (init.removed)
			استمر;
		صحيح وٿي = (init.addr.as_int() - شروعات_وٿي) * ويڪر;
		لكل (صحيح i = 0; i < Get_سائيز(init.معلومات); i++)
			إذا (0 <= i+وٿي && i+وٿي < Get_سائيز(init_معلومات) && init.en[i % ويڪر] == State::S1)
				init_معلومات.سيٽ(i+وٿي, init.معلومات[i]);
	}
	أرجع init_معلومات;
}

فراغ Mem::check() {
	صحيح max_wide_log2 = 0;
	لكل (خودڪار &پورٽ : rd_ports) {
		إذا (پورٽ.removed)
			استمر;
		log_assert(Get_سائيز(پورٽ.clk) == 1);
		log_assert(Get_سائيز(پورٽ.en) == 1);
		log_assert(Get_سائيز(پورٽ.arst) == 1);
		log_assert(Get_سائيز(پورٽ.srst) == 1);
		log_assert(Get_سائيز(پورٽ.addr) >= پورٽ.wide_log2);
		log_assert(Get_سائيز(پورٽ.معلومات) == (ويڪر << پورٽ.wide_log2));
		log_assert(Get_سائيز(پورٽ.init_قدر) == (ويڪر << پورٽ.wide_log2));
		log_assert(Get_سائيز(پورٽ.arst_قدر) == (ويڪر << پورٽ.wide_log2));
		log_assert(Get_سائيز(پورٽ.srst_قدر) == (ويڪر << پورٽ.wide_log2));
		إذا (!پورٽ.clk_enable) {
			log_assert(پورٽ.en == State::S1);
			log_assert(پورٽ.arst == State::S0);
			log_assert(پورٽ.srst == State::S0);
		}
		لكل (صحيح j = 0; j < پورٽ.wide_log2; j++) {
			log_assert(پورٽ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٽ.wide_log2);
		log_assert(Get_سائيز(پورٽ.transparency_نقاب) == Get_سائيز(wr_ports));
		log_assert(Get_سائيز(پورٽ.collision_x_نقاب) == Get_سائيز(wr_ports));
		لكل (صحيح j = 0; j < Get_سائيز(wr_ports); j++) {
			خودڪار &wport = wr_ports[j];
			إذا ((پورٽ.transparency_نقاب[j] || پورٽ.collision_x_نقاب[j]) && !wport.removed) {
				log_assert(پورٽ.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(پورٽ.clk == wport.clk);
				log_assert(پورٽ.clk_polarity == wport.clk_polarity);
			}
			log_assert(!پورٽ.transparency_نقاب[j] || !پورٽ.collision_x_نقاب[j]);
		}
	}
	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
		خودڪار &پورٽ = wr_ports[i];
		إذا (پورٽ.removed)
			استمر;
		log_assert(Get_سائيز(پورٽ.clk) == 1);
		log_assert(Get_سائيز(پورٽ.en) == (ويڪر << پورٽ.wide_log2));
		log_assert(Get_سائيز(پورٽ.معلومات) == (ويڪر << پورٽ.wide_log2));
		log_assert(Get_سائيز(پورٽ.addr) >= پورٽ.wide_log2);
		لكل (صحيح j = 0; j < پورٽ.wide_log2; j++) {
			log_assert(پورٽ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورٽ.wide_log2);
		log_assert(Get_سائيز(پورٽ.ترجيح_نقاب) == Get_سائيز(wr_ports));
		لكل (صحيح j = 0; j < Get_سائيز(wr_ports); j++) {
			خودڪار &wport = wr_ports[j];
			إذا (پورٽ.ترجيح_نقاب[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(پورٽ.clk_enable == wport.clk_enable);
				إذا (پورٽ.clk_enable) {
					log_assert(پورٽ.clk == wport.clk);
					log_assert(پورٽ.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	صحيح نقاب = (1 << max_wide_log2) - 1;
	log_assert(!(شروعات_وٿي & نقاب));
	log_assert(!(سائيز & نقاب));
	log_assert(ويڪر != 0);
}

نطاق {

	بنية Mem_اشاريو {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_اشاريو (Module *module) {
			لكل (خودڪار cell: module->cells()) {
				إذا (cell->type.اندريون(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].وجهو(cell);
				وإلا إذا (cell->type.اندريون(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].وجهو(cell);
				وإلا إذا (cell->type.اندريون(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].وجهو(cell);
			}
		}
	};

	Mem mem_from_يادگيري(Module *module, RTLIL::يادگيري *mem, ثابت Mem_اشاريو &اشاريو) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->ويڪر, mem->شروعات_وٿي, mem->سائيز);
		res.packed = خطأ;
		res.mem = mem;
		res.خاصيتون = mem->خاصيتون;
		std::ويڪٽر<منطقي> rd_transparent;
		std::ويڪٽر<صحيح> wr_portid;
		إذا (اشاريو.rd_ports.ڳڻپ(memid)) {
			لكل (خودڪار cell : اشاريو.rd_ports.at(memid)) {
				MemRd mrd;
				منطقي is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.خاصيتون = cell->خاصيتون;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_پورٽ(ID::CLK);
				mrd.en = cell->get_پورٽ(ID::EN);
				mrd.addr = cell->get_پورٽ(ID::ADDR);
				mrd.معلومات = cell->get_پورٽ(ID::معلومات_2);
				mrd.wide_log2 = ceil_log2(Get_سائيز(mrd.معلومات) / mem->ويڪر);
				منطقي transparent = خطأ;
				إذا (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = خطأ;
					mrd.arst_قدر = Const(State::Sx, mem->ويڪر << mrd.wide_log2);
					mrd.srst_قدر = Const(State::Sx, mem->ويڪر << mrd.wide_log2);
					mrd.init_قدر = Const(State::Sx, mem->ويڪر << mrd.wide_log2);
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
					mrd.arst = cell->get_پورٽ(ID::ARST);
					mrd.srst = cell->get_پورٽ(ID::SRST);
				}
				res.rd_ports.ڌڪو_پٺ(mrd);
				rd_transparent.ڌڪو_پٺ(transparent);
			}
		}
		إذا (اشاريو.wr_ports.ڳڻپ(memid)) {
			std::ويڪٽر<std::pair<صحيح, MemWr>> ports;
			لكل (خودڪار cell : اشاريو.wr_ports.at(memid)) {
				MemWr mwr;
				منطقي is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.خاصيتون = cell->خاصيتون;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_پورٽ(ID::CLK);
				mwr.en = cell->get_پورٽ(ID::EN);
				mwr.addr = cell->get_پورٽ(ID::ADDR);
				mwr.معلومات = cell->get_پورٽ(ID::معلومات_2);
				mwr.wide_log2 = ceil_log2(Get_سائيز(mwr.معلومات) / mem->ويڪر);
				ports.ڌڪو_پٺ(std::make_pair(cell->parameters.at(is_compat ? ID::ترجيح : ID::PORTID).as_int(), mwr));
			}
			std::ترتيب_ڏيو(ports.begin(), ports.پڇاڙي(), [](ثابت std::pair<صحيح, MemWr> &a, ثابت std::pair<صحيح, MemWr> &b) { أرجع a.پهريون < b.پهريون; });
			لكل (خودڪار &it : ports) {
				res.wr_ports.ڌڪو_پٺ(it.ٻيو);
				wr_portid.ڌڪو_پٺ(it.پهريون);
			}
			لكل (صحيح i = 0; i < Get_سائيز(res.wr_ports); i++) {
				خودڪار &پورٽ = res.wr_ports[i];
				منطقي is_compat = پورٽ.cell->type == ID($memwr);
				إذا (is_compat) {
					پورٽ.ترجيح_نقاب.resize(Get_سائيز(res.wr_ports));
					لكل (صحيح j = 0; j < i; j++) {
						خودڪار &oport = res.wr_ports[j];
						إذا (پورٽ.clk_enable != oport.clk_enable)
							استمر;
						إذا (پورٽ.clk_enable && پورٽ.clk != oport.clk)
							استمر;
						إذا (پورٽ.clk_enable && پورٽ.clk_polarity != oport.clk_polarity)
							استمر;
						پورٽ.ترجيح_نقاب[j] = سچ;
					}
				} وإلا {
					Const orig_prio_نقاب = پورٽ.cell->parameters.at(ID::ترجيح_نقاب_2);
					لكل (صحيح orig_portid : wr_portid) {
						منطقي has_prio = orig_portid < Get_سائيز(orig_prio_نقاب) && orig_prio_نقاب[orig_portid] == State::S1;
						پورٽ.ترجيح_نقاب.ڌڪو_پٺ(has_prio);
					}
				}
			}
		}
		إذا (اشاريو.inits.ڳڻپ(memid)) {
			std::ويڪٽر<std::pair<صحيح, MemInit>> inits;
			لكل (خودڪار cell : اشاريو.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.خاصيتون = cell->خاصيتون;
				خودڪار addr = cell->get_پورٽ(ID::ADDR);
				خودڪار معلومات = cell->get_پورٽ(ID::معلومات_2);
				إذا (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				إذا (!معلومات.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(معلومات), cell);
				init.addr = addr.as_const();
				init.معلومات = معلومات.as_const();
				إذا (cell->type == ID($meminit_v2)) {
					خودڪار en = cell->get_پورٽ(ID::EN);
					إذا (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} وإلا {
					init.en = RTLIL::Const(State::S1, mem->ويڪر);
				}
				inits.ڌڪو_پٺ(std::make_pair(cell->parameters.at(ID::ترجيح).as_int(), init));
			}
			std::ترتيب_ڏيو(inits.begin(), inits.پڇاڙي(), [](ثابت std::pair<صحيح, MemInit> &a, ثابت std::pair<صحيح, MemInit> &b) { أرجع a.پهريون < b.پهريون; });
			لكل (خودڪار &it : inits)
				res.inits.ڌڪو_پٺ(it.ٻيو);
		}
		لكل (صحيح i = 0; i < Get_سائيز(res.rd_ports); i++) {
			خودڪار &پورٽ = res.rd_ports[i];
			منطقي is_compat = پورٽ.cell->type == ID($memrd);
			إذا (is_compat) {
				پورٽ.transparency_نقاب.resize(Get_سائيز(res.wr_ports));
				پورٽ.collision_x_نقاب.resize(Get_سائيز(res.wr_ports));
				إذا (!rd_transparent[i])
					استمر;
				إذا (!پورٽ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_سائيز(res.wr_ports); j++) {
					خودڪار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورٽ.clk != wport.clk)
						استمر;
					إذا (پورٽ.clk_polarity != wport.clk_polarity)
						استمر;
					پورٽ.transparency_نقاب[j] = سچ;
				}
			} وإلا {
				Const orig_trans_نقاب = پورٽ.cell->parameters.at(ID::TRANSPARENCY_نقاب);
				Const orig_cx_نقاب = پورٽ.cell->parameters.at(ID::COLLISION_X_نقاب);
				لكل (صحيح orig_portid : wr_portid) {
					پورٽ.transparency_نقاب.ڌڪو_پٺ(orig_portid < Get_سائيز(orig_trans_نقاب) && orig_trans_نقاب[orig_portid] == State::S1);
					پورٽ.collision_x_نقاب.ڌڪو_پٺ(orig_portid < Get_سائيز(orig_cx_نقاب) && orig_cx_نقاب[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		أرجع res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().جوڙيو(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::ويڪر_2).as_int(),
			cell->parameters.at(ID::وٿي_2).as_int(),
			cell->parameters.at(ID::سائيز_2).as_int()
		);
		منطقي is_compat = cell->type == ID($mem);
		صحيح abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = سچ;
		res.cell = cell;
		res.خاصيتون = cell->خاصيتون;
		Const &init = cell->parameters.at(ID::INIT);
		إذا (!init.is_fully_undef()) {
			صحيح pos = 0;
			طالما (pos < res.سائيز) {
				Const لفظ = init.ڪڍو_2(pos * res.ويڪر, res.ويڪر, State::Sx);
				إذا (لفظ.is_fully_undef()) {
					pos++;
				} وإلا {
					صحيح epos;
					لكل (epos = pos; epos < res.سائيز; epos++) {
						Const eword = init.ڪڍو_2(epos * res.ويڪر, res.ويڪر, State::Sx);
						إذا (eword.is_fully_undef())
							اكسر;
					}
					MemInit minit;
					minit.addr = res.شروعات_وٿي + pos;
					minit.معلومات = init.ڪڍو_2(pos * res.ويڪر, (epos - pos) * res.ويڪر, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.ويڪر);
					res.inits.ڌڪو_پٺ(minit);
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
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).ڪڍو_2(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).ڪڍو_2(i, 1).as_bool();
			mrd.clk = cell->get_پورٽ(ID::RD_CLK).ڪڍو_2(i, 1);
			mrd.en = cell->get_پورٽ(ID::RD_EN).ڪڍو_2(i, 1);
			mrd.addr = cell->get_پورٽ(ID::RD_ADDR).ڪڍو_2(i * abits, abits);
			mrd.معلومات = cell->get_پورٽ(ID::RD_معلومات).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
			إذا (is_compat) {
				mrd.ce_over_srst = خطأ;
				mrd.arst_قدر = Const(State::Sx, res.ويڪر << mrd.wide_log2);
				mrd.srst_قدر = Const(State::Sx, res.ويڪر << mrd.wide_log2);
				mrd.init_قدر = Const(State::Sx, res.ويڪر << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} وإلا {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).ڪڍو_2(i, 1).as_bool();
				mrd.arst_قدر = cell->parameters.at(ID::RD_ARST_قدر).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
				mrd.srst_قدر = cell->parameters.at(ID::RD_SRST_قدر).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
				mrd.init_قدر = cell->parameters.at(ID::RD_INIT_قدر).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
				mrd.arst = cell->get_پورٽ(ID::RD_ARST).ڪڍو_2(i, 1);
				mrd.srst = cell->get_پورٽ(ID::RD_SRST).ڪڍو_2(i, 1);
			}
			إذا (!is_compat) {
				Const transparency_نقاب = cell->parameters.at(ID::RD_TRANSPARENCY_نقاب).ڪڍو_2(i * n_wr_ports, n_wr_ports);
				Const collision_x_نقاب = cell->parameters.at(ID::RD_COLLISION_X_نقاب).ڪڍو_2(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_نقاب.ڌڪو_پٺ(transparency_نقاب[j] == State::S1);
						mrd.collision_x_نقاب.ڌڪو_پٺ(collision_x_نقاب[j] == State::S1);
					}
			}
			res.rd_ports.ڌڪو_پٺ(mrd);
		}
		لكل (صحيح i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).ڪڍو_2(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).ڪڍو_2(i, 1).as_bool();
			mwr.clk = cell->get_پورٽ(ID::WR_CLK).ڪڍو_2(i, 1);
			mwr.en = cell->get_پورٽ(ID::WR_EN).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
			mwr.addr = cell->get_پورٽ(ID::WR_ADDR).ڪڍو_2(i * abits, abits);
			mwr.معلومات = cell->get_پورٽ(ID::WR_معلومات).ڪڍو_2(i * res.ويڪر, (ni - i) * res.ويڪر);
			إذا (!is_compat) {
				Const ترجيح_نقاب = cell->parameters.at(ID::WR_ترجيح_نقاب).ڪڍو_2(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1)
						mwr.ترجيح_نقاب.ڌڪو_پٺ(ترجيح_نقاب[j] == State::S1);
			}
			res.wr_ports.ڌڪو_پٺ(mwr);
		}
		إذا (is_compat) {
			لكل (صحيح i = 0; i < Get_سائيز(res.wr_ports); i++) {
				خودڪار &پورٽ = res.wr_ports[i];
				پورٽ.ترجيح_نقاب.resize(Get_سائيز(res.wr_ports));
				لكل (صحيح j = 0; j < i; j++) {
					خودڪار &oport = res.wr_ports[j];
					إذا (پورٽ.clk_enable != oport.clk_enable)
						استمر;
					إذا (پورٽ.clk_enable && پورٽ.clk != oport.clk)
						استمر;
					إذا (پورٽ.clk_enable && پورٽ.clk_polarity != oport.clk_polarity)
						استمر;
					پورٽ.ترجيح_نقاب[j] = سچ;
				}
			}
			لكل (صحيح i = 0; i < Get_سائيز(res.rd_ports); i++) {
				خودڪار &پورٽ = res.rd_ports[i];
				پورٽ.transparency_نقاب.resize(Get_سائيز(res.wr_ports));
				پورٽ.collision_x_نقاب.resize(Get_سائيز(res.wr_ports));
				إذا (!cell->parameters.at(ID::RD_TRANSPARENT).ڪڍو_2(i, 1).as_bool())
					استمر;
				إذا (!پورٽ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_سائيز(res.wr_ports); j++) {
					خودڪار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورٽ.clk != wport.clk)
						استمر;
					إذا (پورٽ.clk_polarity != wport.clk_polarity)
						استمر;
					پورٽ.transparency_نقاب[j] = سچ;
				}
			}
		}
		res.check();
		أرجع res;
	}

}

std::ويڪٽر<Mem> Mem::get_all_memories(Module *module) {
	std::ويڪٽر<Mem> res;
	Mem_اشاريو اشاريو(module);
	لكل (خودڪار it: module->memories) {
		res.ڌڪو_پٺ(mem_from_يادگيري(module, it.ٻيو, اشاريو));
	}
	لكل (خودڪار cell: module->cells()) {
		إذا (cell->type.اندريون(ID($mem), ID($mem_v2)))
			res.ڌڪو_پٺ(mem_from_cell(cell));
	}
	أرجع res;
}

std::ويڪٽر<Mem> Mem::get_selected_memories(Module *module) {
	std::ويڪٽر<Mem> res;
	Mem_اشاريو اشاريو(module);
	لكل (خودڪار it: module->memories) {
		إذا (module->design->selected(module, it.ٻيو))
			res.ڌڪو_پٺ(mem_from_يادگيري(module, it.ٻيو, اشاريو));
	}
	لكل (خودڪار cell: module->selected_cells()) {
		إذا (cell->type.اندريون(ID($mem), ID($mem_v2)))
			res.ڌڪو_پٺ(mem_from_cell(cell));
	}
	أرجع res;
}

Cell *Mem::ڪڍو_rdff(صحيح idx, FfInitVals *initvals) {
	MemRd &پورٽ = rd_ports[idx];

	إذا (!پورٽ.clk_enable)
		أرجع خالي_اشارو;

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
	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++)
		إذا (!پورٽ.transparency_نقاب[i] && !wr_ports[i].removed)
			trans_use_addr = خطأ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	إذا (Get_سائيز(wr_ports) == 0)
		trans_use_addr = خطأ;

	إذا (پورٽ.en != State::S1 || پورٽ.srst != State::S0 || پورٽ.arst != State::S0 || !پورٽ.init_قدر.is_fully_undef())
		trans_use_addr = خطأ;

	إذا (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		صحيح ويڪر = 0;
		لكل (صحيح i = 0; i < Get_سائيز(پورٽ.addr); i++)
			إذا (پورٽ.addr[i].wire)
				ويڪر++;

		إذا (ويڪر)
		{
			SigSpec sig_q = module->جوڙيو_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), ويڪر);
			SigSpec sig_d;

			صحيح pos = 0;
			لكل (صحيح i = 0; i < Get_سائيز(پورٽ.addr); i++)
				إذا (پورٽ.addr[i].wire) {
					sig_d.append(پورٽ.addr[i]);
					پورٽ.addr[i] = sig_q[pos++];
				}

			c = module->جوڙيو_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), پورٽ.clk, sig_d, sig_q, پورٽ.clk_polarity);
		} وإلا {
			c = خالي_اشارو;
		}
	}
	وإلا
	{
		log_assert(پورٽ.arst == State::S0 || پورٽ.srst == State::S0);

		SigSpec async_d = module->جوڙيو_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_سائيز(پورٽ.معلومات));
		SigSpec sig_d = async_d;

		لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
			خودڪار &wport = wr_ports[i];
			إذا (wport.removed)
				استمر;
			إذا (پورٽ.transparency_نقاب[i] || پورٽ.collision_x_نقاب[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == پورٽ.clk);
				log_assert(wport.clk_enable == پورٽ.clk_enable);
				صحيح min_wide_log2 = std::min(پورٽ.wide_log2, wport.wide_log2);
				صحيح max_wide_log2 = std::max(پورٽ.wide_log2, wport.wide_log2);
				منطقي wide_write = wport.wide_log2 > پورٽ.wide_log2;
				لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = پورٽ.addr;
					SigSpec waddr = wport.addr;
					إذا (wide_write)
						waddr = wport.sub_addr(sub);
					وإلا
						raddr = پورٽ.sub_addr(sub);
					SigSpec addr_eq;
					إذا (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					صحيح pos = 0;
					صحيح ewidth = ويڪر << min_wide_log2;
					صحيح wsub = wide_write ? sub : 0;
					صحيح rsub = wide_write ? 0 : sub;
					طالما (pos < ewidth) {
						صحيح epos = pos;
						طالما (epos < ewidth && wport.en[epos + wsub * ويڪر] == wport.en[pos + wsub * ويڪر])
							epos++;
						SigSpec cur = sig_d.ڪڍو_2(pos + rsub * ويڪر, epos-pos);
						SigSpec other = پورٽ.transparency_نقاب[i] ? wport.معلومات.ڪڍو_2(pos + wsub * ويڪر, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						إذا (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * ويڪر], addr_eq);
						وإلا
							cond = wport.en[pos + wsub * ويڪر];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * ويڪر, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().جوڙيو(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_معلومات ff(module, initvals, name);
		ff.ويڪر = Get_سائيز(پورٽ.معلومات);
		ff.has_clk = سچ;
		ff.sig_clk = پورٽ.clk;
		ff.pol_clk = پورٽ.clk_polarity;
		إذا (پورٽ.en != State::S1) {
			ff.has_ce = سچ;
			ff.pol_ce = سچ;
			ff.sig_ce = پورٽ.en;
		}
		إذا (پورٽ.arst != State::S0) {
			ff.has_arst = سچ;
			ff.pol_arst = سچ;
			ff.sig_arst = پورٽ.arst;
			ff.val_arst = پورٽ.arst_قدر;
		}
		إذا (پورٽ.srst != State::S0) {
			ff.has_srst = سچ;
			ff.pol_srst = سچ;
			ff.sig_srst = پورٽ.srst;
			ff.val_srst = پورٽ.srst_قدر;
			ff.ce_over_srst = ff.has_ce && پورٽ.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = پورٽ.معلومات;
		ff.val_init = پورٽ.init_قدر;
		پورٽ.معلومات = async_d;
		c = ff.emit();
	}

	إذا (c)
		لاگرٿم("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	پورٽ.en = State::S1;
	پورٽ.clk = State::S0;
	پورٽ.arst = State::S0;
	پورٽ.srst = State::S0;
	پورٽ.clk_enable = خطأ;
	پورٽ.clk_polarity = سچ;
	پورٽ.ce_over_srst = خطأ;
	پورٽ.arst_قدر = Const(State::Sx, Get_سائيز(پورٽ.معلومات));
	پورٽ.srst_قدر = Const(State::Sx, Get_سائيز(پورٽ.معلومات));
	پورٽ.init_قدر = Const(State::Sx, Get_سائيز(پورٽ.معلومات));

	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
		پورٽ.transparency_نقاب[i] = خطأ;
		پورٽ.collision_x_نقاب[i] = خطأ;
	}

	أرجع c;
}

فراغ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::ويڪٽر<MemRd> new_rd_ports;
	std::ويڪٽر<MemWr> new_wr_ports;
	std::ويڪٽر<std::pair<صحيح, صحيح>> new_rd_map;
	std::ويڪٽر<std::pair<صحيح, صحيح>> new_wr_map;
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++) {
		خودڪار &پورٽ = rd_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورٽ.wide_log2); sub++) {
			new_rd_map.ڌڪو_پٺ(std::make_pair(i, sub));
		}
	}
	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
		خودڪار &پورٽ = wr_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورٽ.wide_log2); sub++) {
			new_wr_map.ڌڪو_پٺ(std::make_pair(i, sub));
		}
	}
	لكل (خودڪار &it : new_rd_map) {
		MemRd &orig = rd_ports[it.پهريون];
		MemRd پورٽ = orig;
		إذا (it.ٻيو != 0)
			پورٽ.cell = خالي_اشارو;
		إذا (پورٽ.wide_log2) {
			پورٽ.معلومات = پورٽ.معلومات.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.init_قدر = پورٽ.init_قدر.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.arst_قدر = پورٽ.arst_قدر.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.srst_قدر = پورٽ.srst_قدر.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.addr = پورٽ.sub_addr(it.ٻيو);
			پورٽ.wide_log2 = 0;
		}
		پورٽ.transparency_نقاب.clear();
		پورٽ.collision_x_نقاب.clear();
		لكل (خودڪار &it2 : new_wr_map)
			پورٽ.transparency_نقاب.ڌڪو_پٺ(orig.transparency_نقاب[it2.پهريون]);
		لكل (خودڪار &it2 : new_wr_map)
			پورٽ.collision_x_نقاب.ڌڪو_پٺ(orig.collision_x_نقاب[it2.پهريون]);
		new_rd_ports.ڌڪو_پٺ(پورٽ);
	}
	لكل (خودڪار &it : new_wr_map) {
		MemWr &orig = wr_ports[it.پهريون];
		MemWr پورٽ = orig;
		إذا (it.ٻيو != 0)
			پورٽ.cell = خالي_اشارو;
		إذا (پورٽ.wide_log2) {
			پورٽ.معلومات = پورٽ.معلومات.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.en = پورٽ.en.ڪڍو_2(it.ٻيو * ويڪر, ويڪر);
			پورٽ.addr = پورٽ.sub_addr(it.ٻيو);
			پورٽ.wide_log2 = 0;
		}
		پورٽ.ترجيح_نقاب.clear();
		لكل (خودڪار &it2 : new_wr_map)
			پورٽ.ترجيح_نقاب.ڌڪو_پٺ(orig.ترجيح_نقاب[it2.پهريون]);
		new_wr_ports.ڌڪو_پٺ(پورٽ);
	}
	std::مٽا_سٽا(rd_ports, new_rd_ports);
	std::مٽا_سٽا(wr_ports, new_wr_ports);
}

فراغ Mem::emulate_ترجيح(صحيح idx1, صحيح idx2, FfInitVals *initvals)
{
	خودڪار &port1 = wr_ports[idx1];
	خودڪار &port2 = wr_ports[idx2];
	إذا (!port2.ترجيح_نقاب[idx1])
		أرجع;
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++) {
		خودڪار &rport = rd_ports[i];
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
		صحيح ewidth = ويڪر << min_wide_log2;
		صحيح sub1 = wide1 ? sub : 0;
		صحيح sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ>, Sig_ٻٽو_انگ> cache;
		لكل (صحيح pos = 0; pos < ewidth; pos++) {
			Sig_ٻٽو_انگ &en1 = port1.en[pos + sub1 * ويڪر];
			Sig_ٻٽو_انگ &en2 = port2.en[pos + sub2 * ويڪر];
			std::pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ> ڪنجي(en1, en2);
			إذا (cache.ڳڻپ(ڪنجي)) {
				en1 = cache[ڪنجي];
			} وإلا {
				Sig_ٻٽو_انگ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_ٻٽو_انگ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[ڪنجي] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ترجيح_نقاب[idx1] = خطأ;
}

فراغ Mem::emulate_transparency(صحيح widx, صحيح ridx, FfInitVals *initvals) {
	خودڪار &wport = wr_ports[widx];
	خودڪار &rport = rd_ports[ridx];
	log_assert(rport.transparency_نقاب[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	لكل (صحيح i = Get_سائيز(wr_ports) - 1; i > widx; i--) {
		إذا (wr_ports[i].removed)
			استمر;
		إذا (rport.transparency_نقاب[i] && wr_ports[i].ترجيح_نقاب[widx])
			emulate_transparency(i, ridx, initvals);
	}
	صحيح min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	صحيح max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	منطقي wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->جوڙيو_Wire(NEW_ID, Get_سائيز(wport.معلومات));
	module->جوڙيو_Dffe(NEW_ID, rport.clk, rport.en, wport.معلومات, wdata_q, rport.clk_polarity, سچ);
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
		صحيح ewidth = ويڪر << min_wide_log2;
		صحيح wsub = wide_write ? sub : 0;
		صحيح rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->جوڙيو_Wire(NEW_ID, ewidth);
		طالما (pos < ewidth) {
			صحيح epos = pos;
			طالما (epos < ewidth && wport.en[epos + wsub * ويڪر] == wport.en[pos + wsub * ويڪر])
				epos++;
			SigSpec cond;
			إذا (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * ويڪر], addr_eq);
			وإلا
				cond = wport.en[pos + wsub * ويڪر];
			SigSpec cond_q = module->جوڙيو_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_معلومات ff(module, initvals, NEW_ID);
			ff.ويڪر = 1;
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
			SigSpec cur = rdata_a.ڪڍو_2(pos, epos-pos);
			SigSpec other = wdata_q.ڪڍو_2(pos + wsub * ويڪر, epos-pos);
			SigSpec dest = rport.معلومات.ڪڍو_2(pos + rsub * ويڪر, epos-pos);
			module->جوڙيو_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.معلومات.replace(rsub * ويڪر, rdata_a);
	}
	rport.transparency_نقاب[widx] = خطأ;
	rport.collision_x_نقاب[widx] = سچ;
}

فراغ Mem::prepare_wr_ملايو(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	خودڪار &port1 = wr_ports[idx1];
	خودڪار &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	لكل (صحيح i = 0; i < idx1; i++)
		إذا (port2.ترجيح_نقاب[i])
			port1.ترجيح_نقاب[i] = سچ;
	// If port 2 has priority over a port after port 1, emulate it.
	لكل (صحيح i = idx1 + 1; i < idx2; i++)
		إذا (port2.ترجيح_نقاب[i] && !wr_ports[i].removed)
			emulate_ترجيح(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	لكل (صحيح i = idx2 + 1; i < Get_سائيز(wr_ports); i++) {
		خودڪار &oport = wr_ports[i];
		إذا (oport.ترجيح_نقاب[idx2])
			oport.ترجيح_نقاب[idx1] = سچ;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++) {
		خودڪار &rport = rd_ports[i];
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

فراغ Mem::prepare_rd_ملايو(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	خودڪار &port1 = rd_ports[idx1];
	خودڪار &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	لكل (صحيح i = 0; i < Get_سائيز(wr_ports); i++) {
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
	صحيح delta = شروعات_وٿي & نقاب;
	شروعات_وٿي -= delta;
	سائيز += delta;
	إذا (سائيز & نقاب) {
		سائيز |= نقاب;
		سائيز++;
	}
}

فراغ Mem::widen_wr_پورٽ(صحيح idx, صحيح wide_log2) {
	widen_prep(wide_log2);
	خودڪار &پورٽ = wr_ports[idx];
	log_assert(پورٽ.wide_log2 <= wide_log2);
	إذا (پورٽ.wide_log2 < wide_log2) {
		SigSpec new_معلومات, new_en;
		SigSpec addr_lo = پورٽ.addr.ڪڍو_2(0, wide_log2);
		لكل (صحيح sub = 0; sub < (1 << wide_log2); sub += (1 << پورٽ.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			إذا (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_معلومات.append(پورٽ.معلومات);
				new_en.append(پورٽ.en);
			} وإلا إذا (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_معلومات.append(Const(State::Sx, Get_سائيز(پورٽ.معلومات)));
				new_en.append(Const(State::S0, Get_سائيز(پورٽ.معلومات)));
			} وإلا {
				// May or may not write to this subword.
				new_معلومات.append(پورٽ.معلومات);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_سائيز(پورٽ.معلومات)), پورٽ.en, addr_eq);
				new_en.append(en);
			}
		}
		پورٽ.addr.replace(پورٽ.wide_log2, Const(State::S0, wide_log2 - پورٽ.wide_log2));
		پورٽ.معلومات = new_معلومات;
		پورٽ.en = new_en;
		پورٽ.wide_log2 = wide_log2;
	}
}

فراغ Mem::emulate_rden(صحيح idx, FfInitVals *initvals) {
	خودڪار &پورٽ = rd_ports[idx];
	log_assert(پورٽ.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
	Wire *prev_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
	Wire *sel = module->جوڙيو_Wire(NEW_ID);
	Ff_معلومات ff_sel(module, initvals, NEW_ID);
	Ff_معلومات ff_معلومات(module, initvals, NEW_ID);
	ff_sel.ويڪر = 1;
	ff_sel.has_clk = سچ;
	ff_sel.sig_clk = پورٽ.clk;
	ff_sel.pol_clk = پورٽ.clk_polarity;
	ff_sel.sig_d = پورٽ.en;
	ff_sel.sig_q = sel;
	ff_معلومات.ويڪر = Get_سائيز(پورٽ.معلومات);
	ff_معلومات.has_clk = سچ;
	ff_معلومات.sig_clk = پورٽ.clk;
	ff_معلومات.pol_clk = پورٽ.clk_polarity;
	ff_معلومات.sig_d = پورٽ.معلومات;
	ff_معلومات.sig_q = prev_معلومات;
	إذا (!پورٽ.init_قدر.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_معلومات.val_init = پورٽ.init_قدر;
		پورٽ.init_قدر = Const(State::Sx, Get_سائيز(پورٽ.معلومات));
	} وإلا {
		ff_sel.val_init = State::Sx;
		ff_معلومات.val_init = Const(State::Sx, Get_سائيز(پورٽ.معلومات));
	}
	إذا (پورٽ.arst != State::S0) {
		ff_sel.has_arst = سچ;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = پورٽ.arst;
		ff_sel.pol_arst = سچ;
		ff_معلومات.has_arst = سچ;
		ff_معلومات.val_arst = پورٽ.arst_قدر;
		ff_معلومات.sig_arst = پورٽ.arst;
		ff_معلومات.pol_arst = سچ;
		پورٽ.arst = State::S0;
	}
	إذا (پورٽ.srst != State::S0) {
		log_assert(!پورٽ.ce_over_srst);
		ff_sel.has_srst = سچ;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = پورٽ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.ce_over_srst = خطأ;
		ff_معلومات.has_srst = سچ;
		ff_معلومات.val_srst = پورٽ.srst_قدر;
		ff_معلومات.sig_srst = پورٽ.srst;
		ff_معلومات.pol_srst = سچ;
		ff_معلومات.ce_over_srst = خطأ;
		پورٽ.srst = State::S0;
	}
	ff_sel.emit();
	ff_معلومات.emit();
	module->جوڙيو_Mux(NEW_ID, prev_معلومات, new_معلومات, sel, پورٽ.معلومات);
	پورٽ.معلومات = new_معلومات;
	پورٽ.en = State::S1;
}

فراغ Mem::emulate_reset(صحيح idx, منطقي emu_init, منطقي emu_arst, منطقي emu_srst, FfInitVals *initvals) {
	خودڪار &پورٽ = rd_ports[idx];
	إذا (emu_init && !پورٽ.init_قدر.is_fully_undef()) {
		Wire *sel = module->جوڙيو_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
		ff_sel.ويڪر = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٽ.clk;
		ff_sel.pol_clk = پورٽ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		إذا (پورٽ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٽ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٽ.ce_over_srst;
		}
		إذا (پورٽ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٽ.arst;
			ff_sel.pol_arst = سچ;
			إذا (emu_arst && پورٽ.arst_قدر == پورٽ.init_قدر) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				پورٽ.arst = State::S0;
			} وإلا {
				ff_sel.val_arst = State::S1;
			}
		}
		إذا (پورٽ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٽ.srst;
			ff_sel.pol_srst = سچ;
			إذا (emu_srst && پورٽ.srst_قدر == پورٽ.init_قدر) {
				ff_sel.val_srst = State::S0;
				پورٽ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڙيو_Mux(NEW_ID, پورٽ.init_قدر, new_معلومات, sel, پورٽ.معلومات);
		پورٽ.معلومات = new_معلومات;
		پورٽ.init_قدر = Const(State::Sx, Get_سائيز(پورٽ.معلومات));
	}
	إذا (emu_arst && پورٽ.arst != State::S0) {
		Wire *sel = module->جوڙيو_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
		ff_sel.ويڪر = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٽ.clk;
		ff_sel.pol_clk = پورٽ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورٽ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورٽ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٽ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٽ.ce_over_srst;
		}
		ff_sel.has_arst = سچ;
		ff_sel.sig_arst = پورٽ.arst;
		ff_sel.pol_arst = سچ;
		ff_sel.val_arst = State::S0;
		إذا (پورٽ.srst != State::S0) {
			ff_sel.has_srst = سچ;
			ff_sel.sig_srst = پورٽ.srst;
			ff_sel.pol_srst = سچ;
			إذا (emu_srst && پورٽ.srst_قدر == پورٽ.arst_قدر) {
				ff_sel.val_srst = State::S0;
				پورٽ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جوڙيو_Mux(NEW_ID, پورٽ.arst_قدر, new_معلومات, sel, پورٽ.معلومات);
		پورٽ.معلومات = new_معلومات;
		پورٽ.arst = State::S0;
	}
	إذا (emu_srst && پورٽ.srst != State::S0) {
		Wire *sel = module->جوڙيو_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
		ff_sel.ويڪر = 1;
		ff_sel.has_clk = سچ;
		ff_sel.sig_clk = پورٽ.clk;
		ff_sel.pol_clk = پورٽ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورٽ.init_قدر.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورٽ.en != State::S1) {
			ff_sel.has_ce = سچ;
			ff_sel.sig_ce = پورٽ.en;
			ff_sel.pol_ce = سچ;
			ff_sel.ce_over_srst = پورٽ.ce_over_srst;
		}
		ff_sel.has_srst = سچ;
		ff_sel.sig_srst = پورٽ.srst;
		ff_sel.pol_srst = سچ;
		ff_sel.val_srst = State::S0;
		إذا (پورٽ.arst != State::S0) {
			ff_sel.has_arst = سچ;
			ff_sel.sig_arst = پورٽ.arst;
			ff_sel.pol_arst = سچ;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->جوڙيو_Mux(NEW_ID, پورٽ.srst_قدر, new_معلومات, sel, پورٽ.معلومات);
		پورٽ.معلومات = new_معلومات;
		پورٽ.srst = State::S0;
	}
}

فراغ Mem::emulate_rd_ce_over_srst(صحيح idx) {
	خودڪار &پورٽ = rd_ports[idx];
	log_assert(پورٽ.clk_enable);
	إذا (پورٽ.en == State::S1 || پورٽ.srst == State::S0 || !پورٽ.ce_over_srst) {
		پورٽ.ce_over_srst = خطأ;
		أرجع;
	}
	پورٽ.ce_over_srst = خطأ;
	پورٽ.srst = module->And(NEW_ID, پورٽ.en, پورٽ.srst);
}

فراغ Mem::emulate_rd_srst_over_ce(صحيح idx) {
	خودڪار &پورٽ = rd_ports[idx];
	log_assert(پورٽ.clk_enable);
	إذا (پورٽ.en == State::S1 || پورٽ.srst == State::S0 || پورٽ.ce_over_srst) {
		پورٽ.ce_over_srst = سچ;
		أرجع;
	}
	پورٽ.ce_over_srst = سچ;
	پورٽ.en = module->Or(NEW_ID, پورٽ.en, پورٽ.srst);
}

منطقي Mem::emulate_read_پهريون_ok() {
	إذا (wr_ports.خالي_جايون())
		أرجع خطأ;
	SigSpec clk = wr_ports[0].clk;
	منطقي clk_polarity = wr_ports[0].clk_polarity;
	لكل (خودڪار &پورٽ: wr_ports) {
		إذا (!پورٽ.clk_enable)
			أرجع خطأ;
		إذا (پورٽ.clk != clk)
			أرجع خطأ;
		إذا (پورٽ.clk_polarity != clk_polarity)
			أرجع خطأ;
	}
	منطقي مليو_read_پهريون = خطأ;
	لكل (خودڪار &پورٽ: rd_ports) {
		إذا (!پورٽ.clk_enable)
			أرجع خطأ;
		إذا (پورٽ.clk != clk)
			أرجع خطأ;
		إذا (پورٽ.clk_polarity != clk_polarity)
			أرجع خطأ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		لكل (صحيح j = 0; j < Get_سائيز(wr_ports); j++)
			إذا (!پورٽ.transparency_نقاب[j] && !پورٽ.collision_x_نقاب[j])
				مليو_read_پهريون = سچ;
	}
	أرجع مليو_read_پهريون;
}

فراغ Mem::emulate_read_پهريون(FfInitVals *initvals) {
	log_assert(emulate_read_پهريون_ok());
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_سائيز(wr_ports); j++)
			إذا (rd_ports[i].transparency_نقاب[j])
				emulate_transparency(j, i, initvals);
	لكل (صحيح i = 0; i < Get_سائيز(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_سائيز(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_نقاب[j]);
			rd_ports[i].collision_x_نقاب[j] = خطأ;
			rd_ports[i].transparency_نقاب[j] = سچ;
		}
	لكل (خودڪار &پورٽ: wr_ports) {
		Wire *new_معلومات = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.معلومات));
		Wire *new_addr = module->جوڙيو_Wire(NEW_ID, Get_سائيز(پورٽ.addr));
		خودڪار compressed = پورٽ.compress_en();
		Wire *new_en = module->جوڙيو_Wire(NEW_ID, Get_سائيز(compressed.پهريون));
		Ff_معلومات ff_معلومات(module, initvals, NEW_ID);
		Ff_معلومات ff_addr(module, initvals, NEW_ID);
		Ff_معلومات ff_en(module, initvals, NEW_ID);
		ff_معلومات.ويڪر = Get_سائيز(پورٽ.معلومات);
		ff_معلومات.has_clk = سچ;
		ff_معلومات.sig_clk = پورٽ.clk;
		ff_معلومات.pol_clk = پورٽ.clk_polarity;
		ff_معلومات.sig_d = پورٽ.معلومات;
		ff_معلومات.sig_q = new_معلومات;;
		ff_معلومات.val_init = Const(State::Sx, ff_معلومات.ويڪر);
		ff_معلومات.emit();
		ff_addr.ويڪر = Get_سائيز(پورٽ.addr);
		ff_addr.has_clk = سچ;
		ff_addr.sig_clk = پورٽ.clk;
		ff_addr.pol_clk = پورٽ.clk_polarity;
		ff_addr.sig_d = پورٽ.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.ويڪر);
		ff_addr.emit();
		ff_en.ويڪر = Get_سائيز(compressed.پهريون);
		ff_en.has_clk = سچ;
		ff_en.sig_clk = پورٽ.clk;
		ff_en.pol_clk = پورٽ.clk_polarity;
		ff_en.sig_d = compressed.پهريون;
		ff_en.sig_q = new_en;;
		إذا (inits.خالي_جايون())
			ff_en.val_init = Const(State::Sx, ff_en.ويڪر);
		وإلا
			ff_en.val_init = Const(State::S0, ff_en.ويڪر);
		ff_en.emit();
		پورٽ.معلومات = new_معلومات;
		پورٽ.addr = new_addr;
		پورٽ.en = پورٽ.decompress_en(compressed.ٻيو, new_en);
	}
}

std::pair<SigSpec, std::ويڪٽر<صحيح>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::ويڪٽر<صحيح> swizzle;
	Sig_ٻٽو_انگ prev_ٻٽو_انگ = en[0];
	صحيح idx = 0;
	لكل (خودڪار &ٻٽو_انگ: en) {
		إذا (ٻٽو_انگ != prev_ٻٽو_انگ) {
			sig.append(ٻٽو_انگ);
			prev_ٻٽو_انگ = ٻٽو_انگ;
			idx++;
		}
		swizzle.ڌڪو_پٺ(idx);
	}
	log_assert(idx + 1 == Get_سائيز(sig));
	أرجع {sig, swizzle};
}

SigSpec MemWr::decompress_en(ثابت std::ويڪٽر<صحيح> &swizzle, SigSpec sig) {
	SigSpec res;
	لكل (صحيح i: swizzle)
		res.append(sig[i]);
	أرجع res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->سائيز), mem->ويڪر)
{
	لكل(ثابت خودڪار &init : mem->inits) {
		إذا(init.en.is_fully_zero()) استمر;
		log_assert(init.en.سائيز() == معلومات_ويڪر_2);
		إذا(init.en.is_fully_ones())
			وجهو_concatenated(init.addr.as_int(), init.معلومات);
		وإلا {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t لفظ_3 = init.معلومات.سائيز() / معلومات_ويڪر_2;
			RTLIL::Const معلومات = init.معلومات;
			log_assert(معلومات.سائيز() % معلومات_ويڪر_2 == 0);
			لكل(addr_t i = 0; i < لفظ_3; i++) {
				RTLIL::Const اڳيون = (*هذا)[addr + i];
				لكل(صحيح j = 0; j < معلومات_ويڪر_2; j++)
					إذا(init.en[j] != State::S1)
						معلومات.سيٽ(معلومات_ويڪر_2 * i + j, اڳيون[j]);
			}
			وجهو_concatenated(init.addr.as_int(), معلومات);
		}
	}
}

MemContents::iterator & MemContents::iterator::عامل++() {
	خودڪار it = يادگيري_2->قدرون_2.مٿانهون_bound(_addr);
	إذا(it == يادگيري_2->قدرون_2.پڇاڙي()) {
		يادگيري_2 = خالي_اشارو;
		_addr = ~(addr_t) 0;
	} وإلا
		_addr = it->پهريون;
	أرجع *هذا;
}

فراغ MemContents::check() {
	log_assert(addr_ويڪر_2 > 0 && addr_ويڪر_2 < (صحيح)حجم(addr_t) * 8);
	log_assert(معلومات_ويڪر_2 > 0);
	log_assert(default_قدر.سائيز() == معلومات_ويڪر_2);
	إذا(قدرون_2.خالي_جايون()) أرجع;
	خودڪار it = قدرون_2.begin();
	لكل(;;) {
		log_assert(!it->ٻيو.خالي_جايون());
		log_assert(it->ٻيو.سائيز() % معلومات_ويڪر_2 == 0);
		خودڪار end1 = range_پڇاڙي(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_ويڪر_2));
		log_assert(end1 <= (addr_t)(1<<addr_ويڪر_2));
		إذا(++it == قدرون_2.پڇاڙي())
			اكسر;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

منطقي MemContents::range_شامل_آهي(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	أرجع it != قدرون_2.پڇاڙي() && addr - _range_begin(it) < range_سائيز(it);
}


منطقي MemContents::range_شامل_آهي(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پڇاڙي_addr) ثابت {
	// note that we assume begin_addr <= end_addr
	أرجع it != قدرون_2.پڇاڙي() && _range_begin(it) <= begin_addr && پڇاڙي_addr - _range_begin(it) <= range_سائيز(it);
}

منطقي MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پڇاڙي_addr) ثابت {
	إذا(it == قدرون_2.پڇاڙي() || begin_addr >= پڇاڙي_addr)
		أرجع خطأ;
	خودڪار top1 = range_پڇاڙي(it) - 1;
	خودڪار top2 = پڇاڙي_addr - 1;
	أرجع !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ثابت {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_ويڪر_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	خودڪار it = const_cast<std::map<addr_t, RTLIL::Const> &>(قدرون_2).مٿانهون_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	إذا(it == قدرون_2.begin())
		أرجع it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	خودڪار it_prev = std::ايندڙ(it, -1);
	إذا(range_شامل_آهي(it_prev, addr))
		أرجع it_prev;
	وإلا
		أرجع it;
}

RTLIL::Const MemContents::عامل[](addr_t addr) ثابت {
	خودڪار it = _range_at(addr);
	إذا(range_شامل_آهي(it, addr))
		أرجع it->ٻيو.ڪڍو_2(range_وٿي(it, addr), معلومات_ويڪر_2);
	وإلا
		أرجع default_قدر;
}

MemContents::addr_t MemContents::ڳڻپ_range(addr_t begin_addr, addr_t پڇاڙي_addr) ثابت {
	addr_t ڳڻپ = 0;
	لكل(خودڪار it = _range_at(begin_addr); _range_overlaps(it, begin_addr, پڇاڙي_addr); it++) {
		خودڪار پهريون = std::max(_range_begin(it), begin_addr);
		خودڪار آخري = std::min(range_پڇاڙي(it), پڇاڙي_addr);
		ڳڻپ += آخري - پهريون;
	}
	أرجع ڳڻپ;
}

فراغ MemContents::clear_range(addr_t begin_addr, addr_t پڇاڙي_addr) {
	إذا(begin_addr >= پڇاڙي_addr) أرجع;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	خودڪار begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	خودڪار پڇاڙي_it = قدرون_2.مٿانهون_bound(پڇاڙي_addr - 1);
	إذا(begin_it == پڇاڙي_it)
		أرجع; // nothing to do
	// the last iterator affected is one before the first one not affected
	خودڪار آخري_it = std::ايندڙ(پڇاڙي_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	إذا(range_شامل_آهي(آخري_it, پڇاڙي_addr - 1)) {
		خودڪار new_begin = پڇاڙي_addr;
		خودڪار پڇاڙي = range_پڇاڙي(آخري_it);
		// if there is data past the end address, preserve it by creating a new range
		إذا(new_begin != پڇاڙي)
			پڇاڙي_it = قدرون_2.emplace_hint(آخري_it, new_begin, آخري_it->ٻيو.ڪڍو_2(range_وٿي(آخري_it, new_begin), (range_پڇاڙي(آخري_it) - new_begin) * معلومات_ويڪر_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	إذا(range_شامل_آهي(begin_it, begin_addr)) {
		خودڪار new_پڇاڙي = begin_addr;
		// if there is data before the start address, truncate but don't delete
		إذا(new_پڇاڙي != begin_it->پهريون) {
			begin_it->ٻيو.extu(range_وٿي(begin_it, new_پڇاڙي));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	قدرون_2.erase(begin_it, پڇاڙي_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t پڇاڙي_addr) {
	إذا(begin_addr >= پڇاڙي_addr)
		أرجع قدرون_2.پڇاڙي(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	خودڪار هيٺاهون_it = begin_addr == 0 ? قدرون_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	إذا (range_شامل_آهي(هيٺاهون_it, begin_addr, پڇاڙي_addr))
		أرجع هيٺاهون_it;
	// find the first range containing any addr >= end_addr
	خودڪار مٿانهون_it = _range_at(پڇاڙي_addr);
	// check if either of the two ranges we just found touch our range
	منطقي هيٺاهون_touch = begin_addr > 0 && range_شامل_آهي(هيٺاهون_it, begin_addr - 1);
	منطقي مٿانهون_touch = range_شامل_آهي(مٿانهون_it, پڇاڙي_addr);
	إذا (هيٺاهون_touch && مٿانهون_touch) {
		log_assert (هيٺاهون_it != مٿانهون_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		خودڪار مٿانهون_پڇاڙي = range_پڇاڙي(مٿانهون_it);
		// make range bigger (maybe reserve here instead of resize?)
		هيٺاهون_it->ٻيو.resize(range_وٿي(هيٺاهون_it, مٿانهون_پڇاڙي), State::Sx);
		// copy only the data beyond our range
		std::copy(range_معلومات(مٿانهون_it, پڇاڙي_addr), range_معلومات(مٿانهون_it, مٿانهون_پڇاڙي), range_معلومات(هيٺاهون_it, پڇاڙي_addr));
		// keep lower_it, but delete upper_it
		قدرون_2.erase(std::ايندڙ(هيٺاهون_it), std::ايندڙ(مٿانهون_it));
		أرجع هيٺاهون_it;
	} وإلا إذا (هيٺاهون_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		هيٺاهون_it->ٻيو.resize(range_وٿي(هيٺاهون_it, پڇاڙي_addr), State::Sx);
		// keep lower_it and upper_it
		قدرون_2.erase(std::ايندڙ(هيٺاهون_it), مٿانهون_it);
		أرجع هيٺاهون_it;
	} وإلا إذا (مٿانهون_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const معلومات(State::Sx, (_range_begin(مٿانهون_it) - begin_addr) * معلومات_ويڪر_2);
		معلومات.append(std::move(مٿانهون_it->ٻيو));
		// delete lower_it and upper_it, then reinsert
		قدرون_2.erase(هيٺاهون_it, std::ايندڙ(مٿانهون_it));
		أرجع قدرون_2.emplace(begin_addr, std::move(معلومات)).پهريون;
	} وإلا {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		قدرون_2.erase(هيٺاهون_it, مٿانهون_it);
		أرجع قدرون_2.emplace(begin_addr, RTLIL::Const(State::Sx, (پڇاڙي_addr - begin_addr) * معلومات_ويڪر_2)).پهريون;
	}
}

فراغ MemContents::وجهو_concatenated(addr_t addr, RTLIL::Const ثابت &قدرون) {
	addr_t لفظ_3 = (قدرون.سائيز() + معلومات_ويڪر_2 - 1) / معلومات_ويڪر_2;
	log_assert(addr < (addr_t)(1<<addr_ويڪر_2));
	log_assert(لفظ_3 <= (addr_t)(1<<addr_ويڪر_2) - addr);
	خودڪار it = _reserve_range(addr, addr + لفظ_3);
	خودڪار منزل_begin = range_معلومات(it, addr);
	std::copy(قدرون.begin(), قدرون.پڇاڙي(), منزل_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(منزل_begin + قدرون.سائيز(), منزل_begin + لفظ_3 * معلومات_ويڪر_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &لفظ) {
	خودڪار from_پڇاڙي = لفظ.سائيز() <= معلومات_ويڪر_2 ? لفظ.پڇاڙي() : لفظ.begin() + معلومات_ويڪر_2;
	خودڪار منزل_پڇاڙي = std::copy(لفظ.begin(), from_پڇاڙي, it);
	خودڪار it_ايندڙ = std::ايندڙ(it, معلومات_ويڪر_2);
	std::fill(منزل_پڇاڙي, it_ايندڙ, State::S0);
	أرجع it_ايندڙ;
}
