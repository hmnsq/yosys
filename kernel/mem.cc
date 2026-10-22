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
		cell = اشاره_تهی;
	}
	إذا (mem) {
		module->memories.erase(mem->name);
		حذف mem;
		mem = اشاره_تهی;
	}
	لكل (خودکار &پورت : rd_ports) {
		إذا (پورت.cell) {
			module->remove(پورت.cell);
			پورت.cell = اشاره_تهی;
		}
	}
	لكل (خودکار &پورت : wr_ports) {
		إذا (پورت.cell) {
			module->remove(پورت.cell);
			پورت.cell = اشاره_تهی;
		}
	}
	لكل (خودکار &init : inits) {
		إذا (init.cell) {
			module->remove(init.cell);
			init.cell = اشاره_تهی;
		}
	}
}

فراغ Mem::emit() {
	check();
	std::وکتور<صحيح> rd_چپ;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خودکار &پورت = rd_ports[i];
		إذا (پورت.removed) {
			إذا (پورت.cell) {
				module->remove(پورت.cell);
			}
		} وإلا {
			rd_چپ.افزودن_پشت(i);
		}
	}
	std::وکتور<صحيح> wr_چپ;
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خودکار &پورت = wr_ports[i];
		إذا (پورت.removed) {
			إذا (پورت.cell) {
				module->remove(پورت.cell);
			}
		} وإلا {
			wr_چپ.افزودن_پشت(i);
		}
	}
	std::وکتور<صحيح> init_چپ;
	لكل (صحيح i = 0; i < Get_اندازه(inits); i++) {
		خودکار &init = inits[i];
		إذا (init.removed) {
			إذا (init.cell) {
				module->remove(init.cell);
			}
		} وإلا {
			init_چپ.افزودن_پشت(i);
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(rd_چپ); i++)
		إذا (i != rd_چپ[i])
			std::جابجا(rd_ports[i], rd_ports[rd_چپ[i]]);
	rd_ports.resize(Get_اندازه(rd_چپ));
	لكل (صحيح i = 0; i < Get_اندازه(wr_چپ); i++)
		إذا (i != wr_چپ[i])
			std::جابجا(wr_ports[i], wr_ports[wr_چپ[i]]);
	wr_ports.resize(Get_اندازه(wr_چپ));
	لكل (صحيح i = 0; i < Get_اندازه(init_چپ); i++)
		إذا (i != init_چپ[i])
			std::جابجا(inits[i], inits[init_چپ[i]]);
	inits.resize(Get_اندازه(init_چپ));

	لكل (خودکار &پورت : rd_ports) {
		لكل (صحيح i = 0; i < Get_اندازه(wr_چپ); i++) {
			پورت.transparency_ماسک[i] = پورت.transparency_ماسک[wr_چپ[i]];
			پورت.collision_x_ماسک[i] = پورت.collision_x_ماسک[wr_چپ[i]];
		}
		پورت.transparency_ماسک.resize(Get_اندازه(wr_چپ));
		پورت.collision_x_ماسک.resize(Get_اندازه(wr_چپ));
	}
	لكل (خودکار &پورت : wr_ports) {
		لكل (صحيح i = 0; i < Get_اندازه(wr_چپ); i++)
			پورت.اولویت_ماسک[i] = پورت.اولویت_ماسک[wr_چپ[i]];
		پورت.اولویت_ماسک.resize(Get_اندازه(wr_چپ));
	}

	إذا (packed) {
		إذا (mem) {
			module->memories.erase(mem->name);
			حذف mem;
			mem = اشاره_تهی;
		}
		إذا (!cell) {
			إذا (memid.خالی())
				memid = PooledName(module->design, module->twines().جمع_کن(NEW_ID));
			cell = module->جمع_کن_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->صفات = صفات;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::عرض_2] = Const(عرض);
		cell->parameters[ID::جابه_جایی_2] = Const(شروع_جابه_جایی);
		cell->parameters[ID::اندازه_2] = Const(اندازه);
		SigSpec rd_clk, rd_en, rd_addr, rd_داده;
		SigSpec wr_clk, wr_en, wr_addr, wr_داده;
		SigSpec rd_arst, rd_srst;
		صحيح abits = 0;
		لكل (خودکار &پورت : rd_ports)
			abits = std::max(abits, Get_اندازه(پورت.addr));
		لكل (خودکار &پورت : wr_ports)
			abits = std::max(abits, Get_اندازه(پورت.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::وکتور<صحيح> wr_پورت_xlat;
		لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++)
			لكل (صحيح j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_پورت_xlat.افزودن_پشت(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_ماسک_builder;
		Const::Builder rd_collision_x_ماسک_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_مقدار_builder;
		Const::Builder rd_srst_مقدار_builder;
		Const::Builder rd_init_مقدار_builder;
		لكل (خودکار &پورت : rd_ports) {
			لكل (خودکار attr: پورت.صفات)
				إذا (!cell->has_attribute(attr.اولی))
					cell->صفات.درج(attr);
			إذا (پورت.cell) {
				module->remove(پورت.cell);
				پورت.cell = اشاره_تهی;
			}
			لكل (صحيح sub = 0; sub < (1 << پورت.wide_log2); sub++)
			{
				rd_wide_continuation_builder.افزودن_پشت(State(sub != 0));
				rd_clk_enable_builder.افزودن_پشت(State(پورت.clk_enable));
				rd_clk_polarity_builder.افزودن_پشت(State(پورت.clk_polarity));
				rd_ce_over_srst_builder.افزودن_پشت(State(پورت.ce_over_srst));
				rd_clk.append(پورت.clk);
				rd_arst.append(پورت.arst);
				rd_srst.append(پورت.srst);
				rd_en.append(پورت.en);
				SigSpec addr = پورت.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				rd_addr.append(addr);
				log_assert(Get_اندازه(addr) == abits);
				لكل (خودکار idx : wr_پورت_xlat) {
					rd_transparency_ماسک_builder.افزودن_پشت(State(منطقي(پورت.transparency_ماسک[idx])));
					rd_collision_x_ماسک_builder.افزودن_پشت(State(منطقي(پورت.collision_x_ماسک[idx])));
				}
			}
			rd_داده.append(پورت.داده);
			لكل (خودکار بیت : پورت.arst_مقدار)
				rd_arst_مقدار_builder.افزودن_پشت(بیت);
			لكل (خودکار بیت : پورت.srst_مقدار)
				rd_srst_مقدار_builder.افزودن_پشت(بیت);
			لكل (خودکار بیت : پورت.init_مقدار)
				rd_init_مقدار_builder.افزودن_پشت(بیت);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_ماسک = rd_transparency_ماسک_builder.build();
		Const rd_collision_x_ماسک = rd_collision_x_ماسک_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_مقدار = rd_arst_مقدار_builder.build();
		Const rd_srst_مقدار = rd_srst_مقدار_builder.build();
		Const rd_init_مقدار = rd_init_مقدار_builder.build();
		إذا (rd_ports.خالی()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_مقدار = State::S0;
			rd_srst_مقدار = State::S0;
			rd_init_مقدار = State::S0;
		}
		إذا (rd_ports.خالی() || wr_ports.خالی()) {
			rd_transparency_ماسک = State::S0;
			rd_collision_x_ماسک = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_اندازه(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_ماسک] = rd_transparency_ماسک;
		cell->parameters[ID::RD_COLLISION_X_ماسک] = rd_collision_x_ماسک;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_مقدار] = rd_arst_مقدار;
		cell->parameters[ID::RD_SRST_مقدار] = rd_srst_مقدار;
		cell->parameters[ID::RD_INIT_مقدار] = rd_init_مقدار;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->مجموعه_پورت(ID::RD_CLK, rd_clk);
		cell->مجموعه_پورت(ID::RD_EN, rd_en);
		cell->مجموعه_پورت(ID::RD_ARST, rd_arst);
		cell->مجموعه_پورت(ID::RD_SRST, rd_srst);
		cell->مجموعه_پورت(ID::RD_ADDR, rd_addr);
		cell->مجموعه_پورت(ID::RD_داده, rd_داده);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_اولویت_ماسک_builder;
		لكل (خودکار &پورت : wr_ports) {
			لكل (خودکار attr: پورت.صفات)
				إذا (!cell->has_attribute(attr.اولی))
					cell->صفات.درج(attr);
			إذا (پورت.cell) {
				module->remove(پورت.cell);
				پورت.cell = اشاره_تهی;
			}
			لكل (صحيح sub = 0; sub < (1 << پورت.wide_log2); sub++)
			{
				wr_wide_continuation_builder.افزودن_پشت(State(sub != 0));
				wr_clk_enable_builder.افزودن_پشت(State(پورت.clk_enable));
				wr_clk_polarity_builder.افزودن_پشت(State(پورت.clk_polarity));
				wr_clk.append(پورت.clk);
				لكل (خودکار idx : wr_پورت_xlat)
					wr_اولویت_ماسک_builder.افزودن_پشت(State(منطقي(پورت.اولویت_ماسک[idx])));
				SigSpec addr = پورت.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				wr_addr.append(addr);
				log_assert(Get_اندازه(addr) == abits);
			}
			wr_en.append(پورت.en);
			wr_داده.append(پورت.داده);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_اولویت_ماسک = wr_اولویت_ماسک_builder.build();
		إذا (wr_ports.خالی()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_اولویت_ماسک = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_اندازه(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_اولویت_ماسک] = wr_اولویت_ماسک;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->مجموعه_پورت(ID::WR_CLK, wr_clk);
		cell->مجموعه_پورت(ID::WR_EN, wr_en);
		cell->مجموعه_پورت(ID::WR_ADDR, wr_addr);
		cell->مجموعه_پورت(ID::WR_داده, wr_داده);
		لكل (خودکار &init : inits) {
			لكل (خودکار attr: init.صفات)
				إذا (!cell->has_attribute(attr.اولی))
					cell->صفات.درج(attr);
			إذا (init.cell) {
				module->remove(init.cell);
				init.cell = اشاره_تهی;
			}
		}
		cell->parameters[ID::INIT] = get_init_داده();
	} وإلا {
		إذا (cell) {
			module->remove(cell);
			cell = اشاره_تهی;
		}
		إذا (!mem) {
			إذا (memid.خالی())
				memid = PooledName(module->design, module->twines().جمع_کن(NEW_ID));
			mem = module->جمع_کن_حافظه(memid);
		}
		mem->عرض = عرض;
		mem->شروع_جابه_جایی = شروع_جابه_جایی;
		mem->اندازه = اندازه;
		mem->صفات = صفات;
		لكل (خودکار &پورت : rd_ports) {
			إذا (!پورت.cell)
				پورت.cell = module->جمع_کن_Cell(NEW_ID, ID($memrd_v2));
			پورت.cell->type = ID($memrd_v2);
			پورت.cell->صفات = پورت.صفات;
			پورت.cell->parameters[ID::MEMID] = memid.str();
			پورت.cell->parameters[ID::ABITS] = Get_اندازه(پورت.addr);
			پورت.cell->parameters[ID::عرض_2] = عرض << پورت.wide_log2;
			پورت.cell->parameters[ID::CLK_ENABLE] = پورت.clk_enable;
			پورت.cell->parameters[ID::CLK_POLARITY] = پورت.clk_polarity;
			پورت.cell->parameters[ID::CE_OVER_SRST] = پورت.ce_over_srst;
			پورت.cell->parameters[ID::ARST_مقدار] = پورت.arst_مقدار;
			پورت.cell->parameters[ID::SRST_مقدار] = پورت.srst_مقدار;
			پورت.cell->parameters[ID::INIT_مقدار] = پورت.init_مقدار;
			پورت.cell->parameters[ID::TRANSPARENCY_ماسک] = پورت.transparency_ماسک;
			پورت.cell->parameters[ID::COLLISION_X_ماسک] = پورت.collision_x_ماسک;
			پورت.cell->parameters.erase(ID::TRANSPARENT);
			پورت.cell->مجموعه_پورت(ID::CLK, پورت.clk);
			پورت.cell->مجموعه_پورت(ID::EN, پورت.en);
			پورت.cell->مجموعه_پورت(ID::ARST, پورت.arst);
			پورت.cell->مجموعه_پورت(ID::SRST, پورت.srst);
			پورت.cell->مجموعه_پورت(ID::ADDR, پورت.addr);
			پورت.cell->مجموعه_پورت(ID::داده_2, پورت.داده);
		}
		صحيح idx = 0;
		لكل (خودکار &پورت : wr_ports) {
			إذا (!پورت.cell)
				پورت.cell = module->جمع_کن_Cell(NEW_ID, ID($memwr_v2));
			پورت.cell->type = ID($memwr_v2);
			پورت.cell->صفات = پورت.صفات;
			إذا (پورت.cell->parameters.شمار(ID::اولویت))
				پورت.cell->parameters.erase(ID::اولویت);
			پورت.cell->parameters[ID::MEMID] = memid.str();
			پورت.cell->parameters[ID::ABITS] = Get_اندازه(پورت.addr);
			پورت.cell->parameters[ID::عرض_2] = عرض << پورت.wide_log2;
			پورت.cell->parameters[ID::CLK_ENABLE] = پورت.clk_enable;
			پورت.cell->parameters[ID::CLK_POLARITY] = پورت.clk_polarity;
			پورت.cell->parameters[ID::PORTID] = idx++;
			پورت.cell->parameters[ID::اولویت_ماسک_2] = پورت.اولویت_ماسک;
			پورت.cell->مجموعه_پورت(ID::CLK, پورت.clk);
			پورت.cell->مجموعه_پورت(ID::EN, پورت.en);
			پورت.cell->مجموعه_پورت(ID::ADDR, پورت.addr);
			پورت.cell->مجموعه_پورت(ID::داده_2, پورت.داده);
		}
		idx = 0;
		لكل (خودکار &init : inits) {
			منطقي v2 = !init.en.is_fully_ones();
			إذا (!init.cell)
				init.cell = module->جمع_کن_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			وإلا {
				إذا (!v2)
					init.cell->unset_پورت(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->صفات = init.صفات;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_اندازه(init.addr);
			init.cell->parameters[ID::عرض_2] = عرض;
			init.cell->parameters[ID::واژگان] = Get_اندازه(init.داده) / عرض;
			init.cell->parameters[ID::اولویت] = idx++;
			init.cell->مجموعه_پورت(ID::ADDR, init.addr);
			init.cell->مجموعه_پورت(ID::داده_2, init.داده);
			إذا (v2)
				init.cell->مجموعه_پورت(ID::EN, init.en);
		}
	}
}

فراغ Mem::clear_inits() {
	لكل (خودکار &init : inits)
		init.removed = درست;
}

فراغ Mem::coalesce_inits() {
	// start address -> end address
	std::map<صحيح, صحيح> chunks;
	// Figure out chunk boundaries.
	لكل (خودکار &init : inits) {
		إذا (init.removed)
			استمر;
		منطقي valid = خطأ;
		لكل (خودکار بیت : init.en)
			إذا (بیت == State::S1)
				valid = درست;
		إذا (!valid) {
			init.removed = درست;
			استمر;
		}
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_اندازه(init.داده) / عرض;
		خودکار it_e = chunks.فوقانی_bound(addr_e);
		خودکار it = it_e;
		طالما (it != chunks.begin()) {
			--it;
			إذا (it->دومی < addr) {
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
			إذا (it->اولی < addr)
				addr = it->اولی;
			خودکار it_آخرین = it_e;
			it_آخرین--;
			إذا (it_آخرین->دومی > addr_e)
				addr_e = it_آخرین->دومی;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<صحيح, std::وکتور<صحيح>> inits_by_chunk;
	لكل (صحيح i = 0; i < Get_اندازه(inits); i++) {
		خودکار &init = inits[i];
		إذا (init.removed)
			استمر;
		خودکار it = chunks.فوقانی_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->اولی].افزودن_پشت(i);
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_اندازه(init.داده) / عرض;
		log_assert(addr >= it->اولی && addr_e <= it->دومی);
	}
	// Process each chunk.
	لكل (خودکار &it : inits_by_chunk) {
		صحيح caddr = it.اولی;
		صحيح caddr_e = chunks[caddr];
		خودکار &chunk_inits = it.دومی;
		إذا (Get_اندازه(chunk_inits) == 1) {
			خودکار &init = inits[chunk_inits[0]];
			إذا (!init.en.is_fully_ones()) {
				لكل (صحيح i = 0; i < Get_اندازه(init.داده); i++)
					إذا (init.en[i % عرض] != State::S1)
						init.داده.مجموعه(i, State::Sx);
				init.en = Const(State::S1, عرض);
			}
			استمر;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * عرض);
		لكل (صحيح idx : chunk_inits) {
			خودکار &init = inits[idx];
			صحيح جابه_جایی = (init.addr.as_int() - caddr) * عرض;
			log_assert(جابه_جایی >= 0);
			log_assert(جابه_جایی + Get_اندازه(init.داده) <= Get_اندازه(cdata));
			لكل (صحيح i = 0; i < Get_اندازه(init.داده); i++)
				إذا (init.en[i % عرض] == State::S1)
					cdata.مجموعه(i+جابه_جایی, init.داده[i]);
			init.removed = درست;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.داده = cdata;
		new_init.en = Const(State::S1, عرض);
		inits.افزودن_پشت(new_init);
	}
}

Const Mem::get_init_داده() ثابت {
	Const init_داده(State::Sx, عرض * اندازه);
	لكل (خودکار &init : inits) {
		إذا (init.removed)
			استمر;
		صحيح جابه_جایی = (init.addr.as_int() - شروع_جابه_جایی) * عرض;
		لكل (صحيح i = 0; i < Get_اندازه(init.داده); i++)
			إذا (0 <= i+جابه_جایی && i+جابه_جایی < Get_اندازه(init_داده) && init.en[i % عرض] == State::S1)
				init_داده.مجموعه(i+جابه_جایی, init.داده[i]);
	}
	أرجع init_داده;
}

فراغ Mem::check() {
	صحيح max_wide_log2 = 0;
	لكل (خودکار &پورت : rd_ports) {
		إذا (پورت.removed)
			استمر;
		log_assert(Get_اندازه(پورت.clk) == 1);
		log_assert(Get_اندازه(پورت.en) == 1);
		log_assert(Get_اندازه(پورت.arst) == 1);
		log_assert(Get_اندازه(پورت.srst) == 1);
		log_assert(Get_اندازه(پورت.addr) >= پورت.wide_log2);
		log_assert(Get_اندازه(پورت.داده) == (عرض << پورت.wide_log2));
		log_assert(Get_اندازه(پورت.init_مقدار) == (عرض << پورت.wide_log2));
		log_assert(Get_اندازه(پورت.arst_مقدار) == (عرض << پورت.wide_log2));
		log_assert(Get_اندازه(پورت.srst_مقدار) == (عرض << پورت.wide_log2));
		إذا (!پورت.clk_enable) {
			log_assert(پورت.en == State::S1);
			log_assert(پورت.arst == State::S0);
			log_assert(پورت.srst == State::S0);
		}
		لكل (صحيح j = 0; j < پورت.wide_log2; j++) {
			log_assert(پورت.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورت.wide_log2);
		log_assert(Get_اندازه(پورت.transparency_ماسک) == Get_اندازه(wr_ports));
		log_assert(Get_اندازه(پورت.collision_x_ماسک) == Get_اندازه(wr_ports));
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			إذا ((پورت.transparency_ماسک[j] || پورت.collision_x_ماسک[j]) && !wport.removed) {
				log_assert(پورت.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(پورت.clk == wport.clk);
				log_assert(پورت.clk_polarity == wport.clk_polarity);
			}
			log_assert(!پورت.transparency_ماسک[j] || !پورت.collision_x_ماسک[j]);
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خودکار &پورت = wr_ports[i];
		إذا (پورت.removed)
			استمر;
		log_assert(Get_اندازه(پورت.clk) == 1);
		log_assert(Get_اندازه(پورت.en) == (عرض << پورت.wide_log2));
		log_assert(Get_اندازه(پورت.داده) == (عرض << پورت.wide_log2));
		log_assert(Get_اندازه(پورت.addr) >= پورت.wide_log2);
		لكل (صحيح j = 0; j < پورت.wide_log2; j++) {
			log_assert(پورت.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورت.wide_log2);
		log_assert(Get_اندازه(پورت.اولویت_ماسک) == Get_اندازه(wr_ports));
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			خودکار &wport = wr_ports[j];
			إذا (پورت.اولویت_ماسک[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(پورت.clk_enable == wport.clk_enable);
				إذا (پورت.clk_enable) {
					log_assert(پورت.clk == wport.clk);
					log_assert(پورت.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	صحيح ماسک = (1 << max_wide_log2) - 1;
	log_assert(!(شروع_جابه_جایی & ماسک));
	log_assert(!(اندازه & ماسک));
	log_assert(عرض != 0);
}

نطاق {

	بنية Mem_شاخص {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_شاخص (Module *module) {
			لكل (خودکار cell: module->cells()) {
				إذا (cell->type.درون(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].درج(cell);
				وإلا إذا (cell->type.درون(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].درج(cell);
				وإلا إذا (cell->type.درون(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].درج(cell);
			}
		}
	};

	Mem mem_from_حافظه(Module *module, RTLIL::حافظه *mem, ثابت Mem_شاخص &شاخص) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->عرض, mem->شروع_جابه_جایی, mem->اندازه);
		res.packed = خطأ;
		res.mem = mem;
		res.صفات = mem->صفات;
		std::وکتور<منطقي> rd_transparent;
		std::وکتور<صحيح> wr_portid;
		إذا (شاخص.rd_ports.شمار(memid)) {
			لكل (خودکار cell : شاخص.rd_ports.at(memid)) {
				MemRd mrd;
				منطقي is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.صفات = cell->صفات;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_پورت(ID::CLK);
				mrd.en = cell->get_پورت(ID::EN);
				mrd.addr = cell->get_پورت(ID::ADDR);
				mrd.داده = cell->get_پورت(ID::داده_2);
				mrd.wide_log2 = ceil_log2(Get_اندازه(mrd.داده) / mem->عرض);
				منطقي transparent = خطأ;
				إذا (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = خطأ;
					mrd.arst_مقدار = Const(State::Sx, mem->عرض << mrd.wide_log2);
					mrd.srst_مقدار = Const(State::Sx, mem->عرض << mrd.wide_log2);
					mrd.init_مقدار = Const(State::Sx, mem->عرض << mrd.wide_log2);
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
					mrd.arst_مقدار = cell->parameters.at(ID::ARST_مقدار);
					mrd.srst_مقدار = cell->parameters.at(ID::SRST_مقدار);
					mrd.init_مقدار = cell->parameters.at(ID::INIT_مقدار);
					mrd.arst = cell->get_پورت(ID::ARST);
					mrd.srst = cell->get_پورت(ID::SRST);
				}
				res.rd_ports.افزودن_پشت(mrd);
				rd_transparent.افزودن_پشت(transparent);
			}
		}
		إذا (شاخص.wr_ports.شمار(memid)) {
			std::وکتور<std::pair<صحيح, MemWr>> ports;
			لكل (خودکار cell : شاخص.wr_ports.at(memid)) {
				MemWr mwr;
				منطقي is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.صفات = cell->صفات;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_پورت(ID::CLK);
				mwr.en = cell->get_پورت(ID::EN);
				mwr.addr = cell->get_پورت(ID::ADDR);
				mwr.داده = cell->get_پورت(ID::داده_2);
				mwr.wide_log2 = ceil_log2(Get_اندازه(mwr.داده) / mem->عرض);
				ports.افزودن_پشت(std::make_pair(cell->parameters.at(is_compat ? ID::اولویت : ID::PORTID).as_int(), mwr));
			}
			std::مرتب_سازی(ports.begin(), ports.انتها(), [](ثابت std::pair<صحيح, MemWr> &a, ثابت std::pair<صحيح, MemWr> &b) { أرجع a.اولی < b.اولی; });
			لكل (خودکار &it : ports) {
				res.wr_ports.افزودن_پشت(it.دومی);
				wr_portid.افزودن_پشت(it.اولی);
			}
			لكل (صحيح i = 0; i < Get_اندازه(res.wr_ports); i++) {
				خودکار &پورت = res.wr_ports[i];
				منطقي is_compat = پورت.cell->type == ID($memwr);
				إذا (is_compat) {
					پورت.اولویت_ماسک.resize(Get_اندازه(res.wr_ports));
					لكل (صحيح j = 0; j < i; j++) {
						خودکار &oport = res.wr_ports[j];
						إذا (پورت.clk_enable != oport.clk_enable)
							استمر;
						إذا (پورت.clk_enable && پورت.clk != oport.clk)
							استمر;
						إذا (پورت.clk_enable && پورت.clk_polarity != oport.clk_polarity)
							استمر;
						پورت.اولویت_ماسک[j] = درست;
					}
				} وإلا {
					Const orig_prio_ماسک = پورت.cell->parameters.at(ID::اولویت_ماسک_2);
					لكل (صحيح orig_portid : wr_portid) {
						منطقي has_prio = orig_portid < Get_اندازه(orig_prio_ماسک) && orig_prio_ماسک[orig_portid] == State::S1;
						پورت.اولویت_ماسک.افزودن_پشت(has_prio);
					}
				}
			}
		}
		إذا (شاخص.inits.شمار(memid)) {
			std::وکتور<std::pair<صحيح, MemInit>> inits;
			لكل (خودکار cell : شاخص.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.صفات = cell->صفات;
				خودکار addr = cell->get_پورت(ID::ADDR);
				خودکار داده = cell->get_پورت(ID::داده_2);
				إذا (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				إذا (!داده.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(داده), cell);
				init.addr = addr.as_const();
				init.داده = داده.as_const();
				إذا (cell->type == ID($meminit_v2)) {
					خودکار en = cell->get_پورت(ID::EN);
					إذا (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} وإلا {
					init.en = RTLIL::Const(State::S1, mem->عرض);
				}
				inits.افزودن_پشت(std::make_pair(cell->parameters.at(ID::اولویت).as_int(), init));
			}
			std::مرتب_سازی(inits.begin(), inits.انتها(), [](ثابت std::pair<صحيح, MemInit> &a, ثابت std::pair<صحيح, MemInit> &b) { أرجع a.اولی < b.اولی; });
			لكل (خودکار &it : inits)
				res.inits.افزودن_پشت(it.دومی);
		}
		لكل (صحيح i = 0; i < Get_اندازه(res.rd_ports); i++) {
			خودکار &پورت = res.rd_ports[i];
			منطقي is_compat = پورت.cell->type == ID($memrd);
			إذا (is_compat) {
				پورت.transparency_ماسک.resize(Get_اندازه(res.wr_ports));
				پورت.collision_x_ماسک.resize(Get_اندازه(res.wr_ports));
				إذا (!rd_transparent[i])
					استمر;
				إذا (!پورت.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_اندازه(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورت.clk != wport.clk)
						استمر;
					إذا (پورت.clk_polarity != wport.clk_polarity)
						استمر;
					پورت.transparency_ماسک[j] = درست;
				}
			} وإلا {
				Const orig_trans_ماسک = پورت.cell->parameters.at(ID::TRANSPARENCY_ماسک);
				Const orig_cx_ماسک = پورت.cell->parameters.at(ID::COLLISION_X_ماسک);
				لكل (صحيح orig_portid : wr_portid) {
					پورت.transparency_ماسک.افزودن_پشت(orig_portid < Get_اندازه(orig_trans_ماسک) && orig_trans_ماسک[orig_portid] == State::S1);
					پورت.collision_x_ماسک.افزودن_پشت(orig_portid < Get_اندازه(orig_cx_ماسک) && orig_cx_ماسک[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		أرجع res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().جمع_کن(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::عرض_2).as_int(),
			cell->parameters.at(ID::جابه_جایی_2).as_int(),
			cell->parameters.at(ID::اندازه_2).as_int()
		);
		منطقي is_compat = cell->type == ID($mem);
		صحيح abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = درست;
		res.cell = cell;
		res.صفات = cell->صفات;
		Const &init = cell->parameters.at(ID::INIT);
		إذا (!init.is_fully_undef()) {
			صحيح pos = 0;
			طالما (pos < res.اندازه) {
				Const کلمه = init.استخراج(pos * res.عرض, res.عرض, State::Sx);
				إذا (کلمه.is_fully_undef()) {
					pos++;
				} وإلا {
					صحيح epos;
					لكل (epos = pos; epos < res.اندازه; epos++) {
						Const eword = init.استخراج(epos * res.عرض, res.عرض, State::Sx);
						إذا (eword.is_fully_undef())
							اكسر;
					}
					MemInit minit;
					minit.addr = res.شروع_جابه_جایی + pos;
					minit.داده = init.استخراج(pos * res.عرض, (epos - pos) * res.عرض, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.عرض);
					res.inits.افزودن_پشت(minit);
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
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).استخراج(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).استخراج(i, 1).as_bool();
			mrd.clk = cell->get_پورت(ID::RD_CLK).استخراج(i, 1);
			mrd.en = cell->get_پورت(ID::RD_EN).استخراج(i, 1);
			mrd.addr = cell->get_پورت(ID::RD_ADDR).استخراج(i * abits, abits);
			mrd.داده = cell->get_پورت(ID::RD_داده).استخراج(i * res.عرض, (ni - i) * res.عرض);
			إذا (is_compat) {
				mrd.ce_over_srst = خطأ;
				mrd.arst_مقدار = Const(State::Sx, res.عرض << mrd.wide_log2);
				mrd.srst_مقدار = Const(State::Sx, res.عرض << mrd.wide_log2);
				mrd.init_مقدار = Const(State::Sx, res.عرض << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} وإلا {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).استخراج(i, 1).as_bool();
				mrd.arst_مقدار = cell->parameters.at(ID::RD_ARST_مقدار).استخراج(i * res.عرض, (ni - i) * res.عرض);
				mrd.srst_مقدار = cell->parameters.at(ID::RD_SRST_مقدار).استخراج(i * res.عرض, (ni - i) * res.عرض);
				mrd.init_مقدار = cell->parameters.at(ID::RD_INIT_مقدار).استخراج(i * res.عرض, (ni - i) * res.عرض);
				mrd.arst = cell->get_پورت(ID::RD_ARST).استخراج(i, 1);
				mrd.srst = cell->get_پورت(ID::RD_SRST).استخراج(i, 1);
			}
			إذا (!is_compat) {
				Const transparency_ماسک = cell->parameters.at(ID::RD_TRANSPARENCY_ماسک).استخراج(i * n_wr_ports, n_wr_ports);
				Const collision_x_ماسک = cell->parameters.at(ID::RD_COLLISION_X_ماسک).استخراج(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_ماسک.افزودن_پشت(transparency_ماسک[j] == State::S1);
						mrd.collision_x_ماسک.افزودن_پشت(collision_x_ماسک[j] == State::S1);
					}
			}
			res.rd_ports.افزودن_پشت(mrd);
		}
		لكل (صحيح i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).استخراج(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).استخراج(i, 1).as_bool();
			mwr.clk = cell->get_پورت(ID::WR_CLK).استخراج(i, 1);
			mwr.en = cell->get_پورت(ID::WR_EN).استخراج(i * res.عرض, (ni - i) * res.عرض);
			mwr.addr = cell->get_پورت(ID::WR_ADDR).استخراج(i * abits, abits);
			mwr.داده = cell->get_پورت(ID::WR_داده).استخراج(i * res.عرض, (ni - i) * res.عرض);
			إذا (!is_compat) {
				Const اولویت_ماسک = cell->parameters.at(ID::WR_اولویت_ماسک).استخراج(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1)
						mwr.اولویت_ماسک.افزودن_پشت(اولویت_ماسک[j] == State::S1);
			}
			res.wr_ports.افزودن_پشت(mwr);
		}
		إذا (is_compat) {
			لكل (صحيح i = 0; i < Get_اندازه(res.wr_ports); i++) {
				خودکار &پورت = res.wr_ports[i];
				پورت.اولویت_ماسک.resize(Get_اندازه(res.wr_ports));
				لكل (صحيح j = 0; j < i; j++) {
					خودکار &oport = res.wr_ports[j];
					إذا (پورت.clk_enable != oport.clk_enable)
						استمر;
					إذا (پورت.clk_enable && پورت.clk != oport.clk)
						استمر;
					إذا (پورت.clk_enable && پورت.clk_polarity != oport.clk_polarity)
						استمر;
					پورت.اولویت_ماسک[j] = درست;
				}
			}
			لكل (صحيح i = 0; i < Get_اندازه(res.rd_ports); i++) {
				خودکار &پورت = res.rd_ports[i];
				پورت.transparency_ماسک.resize(Get_اندازه(res.wr_ports));
				پورت.collision_x_ماسک.resize(Get_اندازه(res.wr_ports));
				إذا (!cell->parameters.at(ID::RD_TRANSPARENT).استخراج(i, 1).as_bool())
					استمر;
				إذا (!پورت.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_اندازه(res.wr_ports); j++) {
					خودکار &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورت.clk != wport.clk)
						استمر;
					إذا (پورت.clk_polarity != wport.clk_polarity)
						استمر;
					پورت.transparency_ماسک[j] = درست;
				}
			}
		}
		res.check();
		أرجع res;
	}

}

std::وکتور<Mem> Mem::get_all_memories(Module *module) {
	std::وکتور<Mem> res;
	Mem_شاخص شاخص(module);
	لكل (خودکار it: module->memories) {
		res.افزودن_پشت(mem_from_حافظه(module, it.دومی, شاخص));
	}
	لكل (خودکار cell: module->cells()) {
		إذا (cell->type.درون(ID($mem), ID($mem_v2)))
			res.افزودن_پشت(mem_from_cell(cell));
	}
	أرجع res;
}

std::وکتور<Mem> Mem::get_selected_memories(Module *module) {
	std::وکتور<Mem> res;
	Mem_شاخص شاخص(module);
	لكل (خودکار it: module->memories) {
		إذا (module->design->selected(module, it.دومی))
			res.افزودن_پشت(mem_from_حافظه(module, it.دومی, شاخص));
	}
	لكل (خودکار cell: module->selected_cells()) {
		إذا (cell->type.درون(ID($mem), ID($mem_v2)))
			res.افزودن_پشت(mem_from_cell(cell));
	}
	أرجع res;
}

Cell *Mem::استخراج_rdff(صحيح idx, FfInitVals *initvals) {
	MemRd &پورت = rd_ports[idx];

	إذا (!پورت.clk_enable)
		أرجع اشاره_تهی;

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
	منطقي trans_use_addr = درست;
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++)
		إذا (!پورت.transparency_ماسک[i] && !wr_ports[i].removed)
			trans_use_addr = خطأ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	إذا (Get_اندازه(wr_ports) == 0)
		trans_use_addr = خطأ;

	إذا (پورت.en != State::S1 || پورت.srst != State::S0 || پورت.arst != State::S0 || !پورت.init_مقدار.is_fully_undef())
		trans_use_addr = خطأ;

	إذا (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		صحيح عرض = 0;
		لكل (صحيح i = 0; i < Get_اندازه(پورت.addr); i++)
			إذا (پورت.addr[i].wire)
				عرض++;

		إذا (عرض)
		{
			SigSpec sig_q = module->جمع_کن_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), عرض);
			SigSpec sig_d;

			صحيح pos = 0;
			لكل (صحيح i = 0; i < Get_اندازه(پورت.addr); i++)
				إذا (پورت.addr[i].wire) {
					sig_d.append(پورت.addr[i]);
					پورت.addr[i] = sig_q[pos++];
				}

			c = module->جمع_کن_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), پورت.clk, sig_d, sig_q, پورت.clk_polarity);
		} وإلا {
			c = اشاره_تهی;
		}
	}
	وإلا
	{
		log_assert(پورت.arst == State::S0 || پورت.srst == State::S0);

		SigSpec async_d = module->جمع_کن_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_اندازه(پورت.داده));
		SigSpec sig_d = async_d;

		لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
			خودکار &wport = wr_ports[i];
			إذا (wport.removed)
				استمر;
			إذا (پورت.transparency_ماسک[i] || پورت.collision_x_ماسک[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == پورت.clk);
				log_assert(wport.clk_enable == پورت.clk_enable);
				صحيح min_wide_log2 = std::min(پورت.wide_log2, wport.wide_log2);
				صحيح max_wide_log2 = std::max(پورت.wide_log2, wport.wide_log2);
				منطقي wide_write = wport.wide_log2 > پورت.wide_log2;
				لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = پورت.addr;
					SigSpec waddr = wport.addr;
					إذا (wide_write)
						waddr = wport.sub_addr(sub);
					وإلا
						raddr = پورت.sub_addr(sub);
					SigSpec addr_eq;
					إذا (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					صحيح pos = 0;
					صحيح ewidth = عرض << min_wide_log2;
					صحيح wsub = wide_write ? sub : 0;
					صحيح rsub = wide_write ? 0 : sub;
					طالما (pos < ewidth) {
						صحيح epos = pos;
						طالما (epos < ewidth && wport.en[epos + wsub * عرض] == wport.en[pos + wsub * عرض])
							epos++;
						SigSpec cur = sig_d.استخراج(pos + rsub * عرض, epos-pos);
						SigSpec other = پورت.transparency_ماسک[i] ? wport.داده.استخراج(pos + wsub * عرض, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						إذا (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * عرض], addr_eq);
						وإلا
							cond = wport.en[pos + wsub * عرض];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * عرض, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().جمع_کن(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_داده ff(module, initvals, name);
		ff.عرض = Get_اندازه(پورت.داده);
		ff.has_clk = درست;
		ff.sig_clk = پورت.clk;
		ff.pol_clk = پورت.clk_polarity;
		إذا (پورت.en != State::S1) {
			ff.has_ce = درست;
			ff.pol_ce = درست;
			ff.sig_ce = پورت.en;
		}
		إذا (پورت.arst != State::S0) {
			ff.has_arst = درست;
			ff.pol_arst = درست;
			ff.sig_arst = پورت.arst;
			ff.val_arst = پورت.arst_مقدار;
		}
		إذا (پورت.srst != State::S0) {
			ff.has_srst = درست;
			ff.pol_srst = درست;
			ff.sig_srst = پورت.srst;
			ff.val_srst = پورت.srst_مقدار;
			ff.ce_over_srst = ff.has_ce && پورت.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = پورت.داده;
		ff.val_init = پورت.init_مقدار;
		پورت.داده = async_d;
		c = ff.emit();
	}

	إذا (c)
		لگاریتم("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	پورت.en = State::S1;
	پورت.clk = State::S0;
	پورت.arst = State::S0;
	پورت.srst = State::S0;
	پورت.clk_enable = خطأ;
	پورت.clk_polarity = درست;
	پورت.ce_over_srst = خطأ;
	پورت.arst_مقدار = Const(State::Sx, Get_اندازه(پورت.داده));
	پورت.srst_مقدار = Const(State::Sx, Get_اندازه(پورت.داده));
	پورت.init_مقدار = Const(State::Sx, Get_اندازه(پورت.داده));

	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		پورت.transparency_ماسک[i] = خطأ;
		پورت.collision_x_ماسک[i] = خطأ;
	}

	أرجع c;
}

فراغ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::وکتور<MemRd> new_rd_ports;
	std::وکتور<MemWr> new_wr_ports;
	std::وکتور<std::pair<صحيح, صحيح>> new_rd_map;
	std::وکتور<std::pair<صحيح, صحيح>> new_wr_map;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خودکار &پورت = rd_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورت.wide_log2); sub++) {
			new_rd_map.افزودن_پشت(std::make_pair(i, sub));
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خودکار &پورت = wr_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورت.wide_log2); sub++) {
			new_wr_map.افزودن_پشت(std::make_pair(i, sub));
		}
	}
	لكل (خودکار &it : new_rd_map) {
		MemRd &orig = rd_ports[it.اولی];
		MemRd پورت = orig;
		إذا (it.دومی != 0)
			پورت.cell = اشاره_تهی;
		إذا (پورت.wide_log2) {
			پورت.داده = پورت.داده.استخراج(it.دومی * عرض, عرض);
			پورت.init_مقدار = پورت.init_مقدار.استخراج(it.دومی * عرض, عرض);
			پورت.arst_مقدار = پورت.arst_مقدار.استخراج(it.دومی * عرض, عرض);
			پورت.srst_مقدار = پورت.srst_مقدار.استخراج(it.دومی * عرض, عرض);
			پورت.addr = پورت.sub_addr(it.دومی);
			پورت.wide_log2 = 0;
		}
		پورت.transparency_ماسک.clear();
		پورت.collision_x_ماسک.clear();
		لكل (خودکار &it2 : new_wr_map)
			پورت.transparency_ماسک.افزودن_پشت(orig.transparency_ماسک[it2.اولی]);
		لكل (خودکار &it2 : new_wr_map)
			پورت.collision_x_ماسک.افزودن_پشت(orig.collision_x_ماسک[it2.اولی]);
		new_rd_ports.افزودن_پشت(پورت);
	}
	لكل (خودکار &it : new_wr_map) {
		MemWr &orig = wr_ports[it.اولی];
		MemWr پورت = orig;
		إذا (it.دومی != 0)
			پورت.cell = اشاره_تهی;
		إذا (پورت.wide_log2) {
			پورت.داده = پورت.داده.استخراج(it.دومی * عرض, عرض);
			پورت.en = پورت.en.استخراج(it.دومی * عرض, عرض);
			پورت.addr = پورت.sub_addr(it.دومی);
			پورت.wide_log2 = 0;
		}
		پورت.اولویت_ماسک.clear();
		لكل (خودکار &it2 : new_wr_map)
			پورت.اولویت_ماسک.افزودن_پشت(orig.اولویت_ماسک[it2.اولی]);
		new_wr_ports.افزودن_پشت(پورت);
	}
	std::جابجا(rd_ports, new_rd_ports);
	std::جابجا(wr_ports, new_wr_ports);
}

فراغ Mem::emulate_اولویت(صحيح idx1, صحيح idx2, FfInitVals *initvals)
{
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	إذا (!port2.اولویت_ماسک[idx1])
		أرجع;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		إذا (rport.transparency_ماسک[idx1] && !(rport.transparency_ماسک[idx2] || rport.collision_x_ماسک[idx2]))
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
		صحيح ewidth = عرض << min_wide_log2;
		صحيح sub1 = wide1 ? sub : 0;
		صحيح sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_بیت, Sig_بیت>, Sig_بیت> cache;
		لكل (صحيح pos = 0; pos < ewidth; pos++) {
			Sig_بیت &en1 = port1.en[pos + sub1 * عرض];
			Sig_بیت &en2 = port2.en[pos + sub2 * عرض];
			std::pair<Sig_بیت, Sig_بیت> کلید(en1, en2);
			إذا (cache.شمار(کلید)) {
				en1 = cache[کلید];
			} وإلا {
				Sig_بیت active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_بیت nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[کلید] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.اولویت_ماسک[idx1] = خطأ;
}

فراغ Mem::emulate_transparency(صحيح widx, صحيح ridx, FfInitVals *initvals) {
	خودکار &wport = wr_ports[widx];
	خودکار &rport = rd_ports[ridx];
	log_assert(rport.transparency_ماسک[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	لكل (صحيح i = Get_اندازه(wr_ports) - 1; i > widx; i--) {
		إذا (wr_ports[i].removed)
			استمر;
		إذا (rport.transparency_ماسک[i] && wr_ports[i].اولویت_ماسک[widx])
			emulate_transparency(i, ridx, initvals);
	}
	صحيح min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	صحيح max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	منطقي wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->جمع_کن_Wire(NEW_ID, Get_اندازه(wport.داده));
	module->جمع_کن_Dffe(NEW_ID, rport.clk, rport.en, wport.داده, wdata_q, rport.clk_polarity, درست);
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
		صحيح ewidth = عرض << min_wide_log2;
		صحيح wsub = wide_write ? sub : 0;
		صحيح rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->جمع_کن_Wire(NEW_ID, ewidth);
		طالما (pos < ewidth) {
			صحيح epos = pos;
			طالما (epos < ewidth && wport.en[epos + wsub * عرض] == wport.en[pos + wsub * عرض])
				epos++;
			SigSpec cond;
			إذا (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * عرض], addr_eq);
			وإلا
				cond = wport.en[pos + wsub * عرض];
			SigSpec cond_q = module->جمع_کن_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_داده ff(module, initvals, NEW_ID);
			ff.عرض = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = درست;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			إذا (rport.en != State::S1) {
				ff.has_ce = درست;
				ff.sig_ce = rport.en;
				ff.pol_ce = درست;
			}
			إذا (rport.arst != State::S0) {
				ff.has_arst = درست;
				ff.sig_arst = rport.arst;
				ff.pol_arst = درست;
				ff.val_arst = State::S0;
			}
			إذا (rport.srst != State::S0) {
				ff.has_srst = درست;
				ff.sig_srst = rport.srst;
				ff.pol_srst = درست;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			إذا (!rport.init_مقدار.is_fully_undef())
				ff.val_init = State::S0;
			وإلا
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.استخراج(pos, epos-pos);
			SigSpec other = wdata_q.استخراج(pos + wsub * عرض, epos-pos);
			SigSpec dest = rport.داده.استخراج(pos + rsub * عرض, epos-pos);
			module->جمع_کن_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.داده.replace(rsub * عرض, rdata_a);
	}
	rport.transparency_ماسک[widx] = خطأ;
	rport.collision_x_ماسک[widx] = درست;
}

فراغ Mem::prepare_wr_ادغام(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	خودکار &port1 = wr_ports[idx1];
	خودکار &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	لكل (صحيح i = 0; i < idx1; i++)
		إذا (port2.اولویت_ماسک[i])
			port1.اولویت_ماسک[i] = درست;
	// If port 2 has priority over a port after port 1, emulate it.
	لكل (صحيح i = idx1 + 1; i < idx2; i++)
		إذا (port2.اولویت_ماسک[i] && !wr_ports[i].removed)
			emulate_اولویت(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	لكل (صحيح i = idx2 + 1; i < Get_اندازه(wr_ports); i++) {
		خودکار &oport = wr_ports[i];
		إذا (oport.اولویت_ماسک[idx2])
			oport.اولویت_ماسک[idx1] = درست;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خودکار &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		// If collision already undefined with both ports, it's fine.
		إذا (rport.collision_x_ماسک[idx1] && rport.collision_x_ماسک[idx2])
			استمر;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		إذا (rport.collision_x_ماسک[idx1]) {
			rport.collision_x_ماسک[idx1] = خطأ;
			rport.transparency_ماسک[idx1] = rport.transparency_ماسک[idx2];
			استمر;
		}
		إذا (rport.collision_x_ماسک[idx2]) {
			rport.collision_x_ماسک[idx2] = خطأ;
			rport.transparency_ماسک[idx2] = rport.transparency_ماسک[idx1];
			استمر;
		}
		// If transparent with both ports, also fine.
		إذا (rport.transparency_ماسک[idx1] && rport.transparency_ماسک[idx2])
			استمر;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		إذا (rport.transparency_ماسک[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_ماسک[idx1] = خطأ;
			استمر;
		}
		إذا (rport.transparency_ماسک[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_ماسک[idx2] = خطأ;
			استمر;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

فراغ Mem::prepare_rd_ادغام(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	خودکار &port1 = rd_ports[idx1];
	خودکار &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		إذا (wr_ports[i].removed)
			استمر;
		// Both ports undefined, OK.
		إذا (port1.collision_x_ماسک[i] && port2.collision_x_ماسک[i])
			استمر;
		// Only one port undefined — change its behavior
		// to align with the other port.
		إذا (port1.collision_x_ماسک[i]) {
			port1.collision_x_ماسک[i] = خطأ;
			port1.transparency_ماسک[i] = port2.transparency_ماسک[i];
			استمر;
		}
		إذا (port2.collision_x_ماسک[i]) {
			port2.collision_x_ماسک[i] = خطأ;
			port2.transparency_ماسک[i] = port1.transparency_ماسک[i];
			استمر;
		}
		// Both ports transparent, OK.
		إذا (port1.transparency_ماسک[i] && port2.transparency_ماسک[i])
			استمر;
		// Only one port transparent — emulate transparency
		// on the other.
		إذا (port1.transparency_ماسک[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_ماسک[i] = خطأ;
			استمر;
		}
		إذا (port2.transparency_ماسک[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_ماسک[i] = خطأ;
			استمر;
		}
		// No ports transparent, OK.
	}

}

فراغ Mem::widen_prep(صحيح wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	صحيح ماسک = ((1 << wide_log2) - 1);
	صحيح delta = شروع_جابه_جایی & ماسک;
	شروع_جابه_جایی -= delta;
	اندازه += delta;
	إذا (اندازه & ماسک) {
		اندازه |= ماسک;
		اندازه++;
	}
}

فراغ Mem::widen_wr_پورت(صحيح idx, صحيح wide_log2) {
	widen_prep(wide_log2);
	خودکار &پورت = wr_ports[idx];
	log_assert(پورت.wide_log2 <= wide_log2);
	إذا (پورت.wide_log2 < wide_log2) {
		SigSpec new_داده, new_en;
		SigSpec addr_lo = پورت.addr.استخراج(0, wide_log2);
		لكل (صحيح sub = 0; sub < (1 << wide_log2); sub += (1 << پورت.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			إذا (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_داده.append(پورت.داده);
				new_en.append(پورت.en);
			} وإلا إذا (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_داده.append(Const(State::Sx, Get_اندازه(پورت.داده)));
				new_en.append(Const(State::S0, Get_اندازه(پورت.داده)));
			} وإلا {
				// May or may not write to this subword.
				new_داده.append(پورت.داده);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_اندازه(پورت.داده)), پورت.en, addr_eq);
				new_en.append(en);
			}
		}
		پورت.addr.replace(پورت.wide_log2, Const(State::S0, wide_log2 - پورت.wide_log2));
		پورت.داده = new_داده;
		پورت.en = new_en;
		پورت.wide_log2 = wide_log2;
	}
}

فراغ Mem::emulate_rden(صحيح idx, FfInitVals *initvals) {
	خودکار &پورت = rd_ports[idx];
	log_assert(پورت.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
	Wire *prev_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
	Wire *sel = module->جمع_کن_Wire(NEW_ID);
	Ff_داده ff_sel(module, initvals, NEW_ID);
	Ff_داده ff_داده(module, initvals, NEW_ID);
	ff_sel.عرض = 1;
	ff_sel.has_clk = درست;
	ff_sel.sig_clk = پورت.clk;
	ff_sel.pol_clk = پورت.clk_polarity;
	ff_sel.sig_d = پورت.en;
	ff_sel.sig_q = sel;
	ff_داده.عرض = Get_اندازه(پورت.داده);
	ff_داده.has_clk = درست;
	ff_داده.sig_clk = پورت.clk;
	ff_داده.pol_clk = پورت.clk_polarity;
	ff_داده.sig_d = پورت.داده;
	ff_داده.sig_q = prev_داده;
	إذا (!پورت.init_مقدار.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_داده.val_init = پورت.init_مقدار;
		پورت.init_مقدار = Const(State::Sx, Get_اندازه(پورت.داده));
	} وإلا {
		ff_sel.val_init = State::Sx;
		ff_داده.val_init = Const(State::Sx, Get_اندازه(پورت.داده));
	}
	إذا (پورت.arst != State::S0) {
		ff_sel.has_arst = درست;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = پورت.arst;
		ff_sel.pol_arst = درست;
		ff_داده.has_arst = درست;
		ff_داده.val_arst = پورت.arst_مقدار;
		ff_داده.sig_arst = پورت.arst;
		ff_داده.pol_arst = درست;
		پورت.arst = State::S0;
	}
	إذا (پورت.srst != State::S0) {
		log_assert(!پورت.ce_over_srst);
		ff_sel.has_srst = درست;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = پورت.srst;
		ff_sel.pol_srst = درست;
		ff_sel.ce_over_srst = خطأ;
		ff_داده.has_srst = درست;
		ff_داده.val_srst = پورت.srst_مقدار;
		ff_داده.sig_srst = پورت.srst;
		ff_داده.pol_srst = درست;
		ff_داده.ce_over_srst = خطأ;
		پورت.srst = State::S0;
	}
	ff_sel.emit();
	ff_داده.emit();
	module->جمع_کن_Mux(NEW_ID, prev_داده, new_داده, sel, پورت.داده);
	پورت.داده = new_داده;
	پورت.en = State::S1;
}

فراغ Mem::emulate_reset(صحيح idx, منطقي emu_init, منطقي emu_arst, منطقي emu_srst, FfInitVals *initvals) {
	خودکار &پورت = rd_ports[idx];
	إذا (emu_init && !پورت.init_مقدار.is_fully_undef()) {
		Wire *sel = module->جمع_کن_Wire(NEW_ID);
		Ff_داده ff_sel(module, initvals, NEW_ID);
		Wire *new_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
		ff_sel.عرض = 1;
		ff_sel.has_clk = درست;
		ff_sel.sig_clk = پورت.clk;
		ff_sel.pol_clk = پورت.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		إذا (پورت.en != State::S1) {
			ff_sel.has_ce = درست;
			ff_sel.sig_ce = پورت.en;
			ff_sel.pol_ce = درست;
			ff_sel.ce_over_srst = پورت.ce_over_srst;
		}
		إذا (پورت.arst != State::S0) {
			ff_sel.has_arst = درست;
			ff_sel.sig_arst = پورت.arst;
			ff_sel.pol_arst = درست;
			إذا (emu_arst && پورت.arst_مقدار == پورت.init_مقدار) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				پورت.arst = State::S0;
			} وإلا {
				ff_sel.val_arst = State::S1;
			}
		}
		إذا (پورت.srst != State::S0) {
			ff_sel.has_srst = درست;
			ff_sel.sig_srst = پورت.srst;
			ff_sel.pol_srst = درست;
			إذا (emu_srst && پورت.srst_مقدار == پورت.init_مقدار) {
				ff_sel.val_srst = State::S0;
				پورت.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جمع_کن_Mux(NEW_ID, پورت.init_مقدار, new_داده, sel, پورت.داده);
		پورت.داده = new_داده;
		پورت.init_مقدار = Const(State::Sx, Get_اندازه(پورت.داده));
	}
	إذا (emu_arst && پورت.arst != State::S0) {
		Wire *sel = module->جمع_کن_Wire(NEW_ID);
		Ff_داده ff_sel(module, initvals, NEW_ID);
		Wire *new_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
		ff_sel.عرض = 1;
		ff_sel.has_clk = درست;
		ff_sel.sig_clk = پورت.clk;
		ff_sel.pol_clk = پورت.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورت.init_مقدار.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورت.en != State::S1) {
			ff_sel.has_ce = درست;
			ff_sel.sig_ce = پورت.en;
			ff_sel.pol_ce = درست;
			ff_sel.ce_over_srst = پورت.ce_over_srst;
		}
		ff_sel.has_arst = درست;
		ff_sel.sig_arst = پورت.arst;
		ff_sel.pol_arst = درست;
		ff_sel.val_arst = State::S0;
		إذا (پورت.srst != State::S0) {
			ff_sel.has_srst = درست;
			ff_sel.sig_srst = پورت.srst;
			ff_sel.pol_srst = درست;
			إذا (emu_srst && پورت.srst_مقدار == پورت.arst_مقدار) {
				ff_sel.val_srst = State::S0;
				پورت.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جمع_کن_Mux(NEW_ID, پورت.arst_مقدار, new_داده, sel, پورت.داده);
		پورت.داده = new_داده;
		پورت.arst = State::S0;
	}
	إذا (emu_srst && پورت.srst != State::S0) {
		Wire *sel = module->جمع_کن_Wire(NEW_ID);
		Ff_داده ff_sel(module, initvals, NEW_ID);
		Wire *new_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
		ff_sel.عرض = 1;
		ff_sel.has_clk = درست;
		ff_sel.sig_clk = پورت.clk;
		ff_sel.pol_clk = پورت.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورت.init_مقدار.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورت.en != State::S1) {
			ff_sel.has_ce = درست;
			ff_sel.sig_ce = پورت.en;
			ff_sel.pol_ce = درست;
			ff_sel.ce_over_srst = پورت.ce_over_srst;
		}
		ff_sel.has_srst = درست;
		ff_sel.sig_srst = پورت.srst;
		ff_sel.pol_srst = درست;
		ff_sel.val_srst = State::S0;
		إذا (پورت.arst != State::S0) {
			ff_sel.has_arst = درست;
			ff_sel.sig_arst = پورت.arst;
			ff_sel.pol_arst = درست;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->جمع_کن_Mux(NEW_ID, پورت.srst_مقدار, new_داده, sel, پورت.داده);
		پورت.داده = new_داده;
		پورت.srst = State::S0;
	}
}

فراغ Mem::emulate_rd_ce_over_srst(صحيح idx) {
	خودکار &پورت = rd_ports[idx];
	log_assert(پورت.clk_enable);
	إذا (پورت.en == State::S1 || پورت.srst == State::S0 || !پورت.ce_over_srst) {
		پورت.ce_over_srst = خطأ;
		أرجع;
	}
	پورت.ce_over_srst = خطأ;
	پورت.srst = module->And(NEW_ID, پورت.en, پورت.srst);
}

فراغ Mem::emulate_rd_srst_over_ce(صحيح idx) {
	خودکار &پورت = rd_ports[idx];
	log_assert(پورت.clk_enable);
	إذا (پورت.en == State::S1 || پورت.srst == State::S0 || پورت.ce_over_srst) {
		پورت.ce_over_srst = درست;
		أرجع;
	}
	پورت.ce_over_srst = درست;
	پورت.en = module->Or(NEW_ID, پورت.en, پورت.srst);
}

منطقي Mem::emulate_read_اولی_ok() {
	إذا (wr_ports.خالی())
		أرجع خطأ;
	SigSpec clk = wr_ports[0].clk;
	منطقي clk_polarity = wr_ports[0].clk_polarity;
	لكل (خودکار &پورت: wr_ports) {
		إذا (!پورت.clk_enable)
			أرجع خطأ;
		إذا (پورت.clk != clk)
			أرجع خطأ;
		إذا (پورت.clk_polarity != clk_polarity)
			أرجع خطأ;
	}
	منطقي یافت_read_اولی = خطأ;
	لكل (خودکار &پورت: rd_ports) {
		إذا (!پورت.clk_enable)
			أرجع خطأ;
		إذا (پورت.clk != clk)
			أرجع خطأ;
		إذا (پورت.clk_polarity != clk_polarity)
			أرجع خطأ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++)
			إذا (!پورت.transparency_ماسک[j] && !پورت.collision_x_ماسک[j])
				یافت_read_اولی = درست;
	}
	أرجع یافت_read_اولی;
}

فراغ Mem::emulate_read_اولی(FfInitVals *initvals) {
	log_assert(emulate_read_اولی_ok());
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++)
			إذا (rd_ports[i].transparency_ماسک[j])
				emulate_transparency(j, i, initvals);
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_ماسک[j]);
			rd_ports[i].collision_x_ماسک[j] = خطأ;
			rd_ports[i].transparency_ماسک[j] = درست;
		}
	لكل (خودکار &پورت: wr_ports) {
		Wire *new_داده = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.داده));
		Wire *new_addr = module->جمع_کن_Wire(NEW_ID, Get_اندازه(پورت.addr));
		خودکار compressed = پورت.compress_en();
		Wire *new_en = module->جمع_کن_Wire(NEW_ID, Get_اندازه(compressed.اولی));
		Ff_داده ff_داده(module, initvals, NEW_ID);
		Ff_داده ff_addr(module, initvals, NEW_ID);
		Ff_داده ff_en(module, initvals, NEW_ID);
		ff_داده.عرض = Get_اندازه(پورت.داده);
		ff_داده.has_clk = درست;
		ff_داده.sig_clk = پورت.clk;
		ff_داده.pol_clk = پورت.clk_polarity;
		ff_داده.sig_d = پورت.داده;
		ff_داده.sig_q = new_داده;;
		ff_داده.val_init = Const(State::Sx, ff_داده.عرض);
		ff_داده.emit();
		ff_addr.عرض = Get_اندازه(پورت.addr);
		ff_addr.has_clk = درست;
		ff_addr.sig_clk = پورت.clk;
		ff_addr.pol_clk = پورت.clk_polarity;
		ff_addr.sig_d = پورت.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.عرض);
		ff_addr.emit();
		ff_en.عرض = Get_اندازه(compressed.اولی);
		ff_en.has_clk = درست;
		ff_en.sig_clk = پورت.clk;
		ff_en.pol_clk = پورت.clk_polarity;
		ff_en.sig_d = compressed.اولی;
		ff_en.sig_q = new_en;;
		إذا (inits.خالی())
			ff_en.val_init = Const(State::Sx, ff_en.عرض);
		وإلا
			ff_en.val_init = Const(State::S0, ff_en.عرض);
		ff_en.emit();
		پورت.داده = new_داده;
		پورت.addr = new_addr;
		پورت.en = پورت.decompress_en(compressed.دومی, new_en);
	}
}

std::pair<SigSpec, std::وکتور<صحيح>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::وکتور<صحيح> swizzle;
	Sig_بیت prev_بیت = en[0];
	صحيح idx = 0;
	لكل (خودکار &بیت: en) {
		إذا (بیت != prev_بیت) {
			sig.append(بیت);
			prev_بیت = بیت;
			idx++;
		}
		swizzle.افزودن_پشت(idx);
	}
	log_assert(idx + 1 == Get_اندازه(sig));
	أرجع {sig, swizzle};
}

SigSpec MemWr::decompress_en(ثابت std::وکتور<صحيح> &swizzle, SigSpec sig) {
	SigSpec res;
	لكل (صحيح i: swizzle)
		res.append(sig[i]);
	أرجع res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->اندازه), mem->عرض)
{
	لكل(ثابت خودکار &init : mem->inits) {
		إذا(init.en.is_fully_zero()) استمر;
		log_assert(init.en.اندازه() == داده_عرض_2);
		إذا(init.en.is_fully_ones())
			درج_concatenated(init.addr.as_int(), init.داده);
		وإلا {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t واژگان_2 = init.داده.اندازه() / داده_عرض_2;
			RTLIL::Const داده = init.داده;
			log_assert(داده.اندازه() % داده_عرض_2 == 0);
			لكل(addr_t i = 0; i < واژگان_2; i++) {
				RTLIL::Const قبلی = (*هذا)[addr + i];
				لكل(صحيح j = 0; j < داده_عرض_2; j++)
					إذا(init.en[j] != State::S1)
						داده.مجموعه(داده_عرض_2 * i + j, قبلی[j]);
			}
			درج_concatenated(init.addr.as_int(), داده);
		}
	}
}

MemContents::iterator & MemContents::iterator::عملگر++() {
	خودکار it = حافظه_2->مقادیر_2.فوقانی_bound(_addr);
	إذا(it == حافظه_2->مقادیر_2.انتها()) {
		حافظه_2 = اشاره_تهی;
		_addr = ~(addr_t) 0;
	} وإلا
		_addr = it->اولی;
	أرجع *هذا;
}

فراغ MemContents::check() {
	log_assert(addr_عرض_2 > 0 && addr_عرض_2 < (صحيح)حجم(addr_t) * 8);
	log_assert(داده_عرض_2 > 0);
	log_assert(default_مقدار.اندازه() == داده_عرض_2);
	إذا(مقادیر_2.خالی()) أرجع;
	خودکار it = مقادیر_2.begin();
	لكل(;;) {
		log_assert(!it->دومی.خالی());
		log_assert(it->دومی.اندازه() % داده_عرض_2 == 0);
		خودکار end1 = range_انتها(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_عرض_2));
		log_assert(end1 <= (addr_t)(1<<addr_عرض_2));
		إذا(++it == مقادیر_2.انتها())
			اكسر;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

منطقي MemContents::range_شامل_است(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	أرجع it != مقادیر_2.انتها() && addr - _range_begin(it) < range_اندازه(it);
}


منطقي MemContents::range_شامل_است(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتها_addr) ثابت {
	// note that we assume begin_addr <= end_addr
	أرجع it != مقادیر_2.انتها() && _range_begin(it) <= begin_addr && انتها_addr - _range_begin(it) <= range_اندازه(it);
}

منطقي MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتها_addr) ثابت {
	إذا(it == مقادیر_2.انتها() || begin_addr >= انتها_addr)
		أرجع خطأ;
	خودکار top1 = range_انتها(it) - 1;
	خودکار top2 = انتها_addr - 1;
	أرجع !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ثابت {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_عرض_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	خودکار it = const_cast<std::map<addr_t, RTLIL::Const> &>(مقادیر_2).فوقانی_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	إذا(it == مقادیر_2.begin())
		أرجع it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	خودکار it_prev = std::بعدی(it, -1);
	إذا(range_شامل_است(it_prev, addr))
		أرجع it_prev;
	وإلا
		أرجع it;
}

RTLIL::Const MemContents::عملگر[](addr_t addr) ثابت {
	خودکار it = _range_at(addr);
	إذا(range_شامل_است(it, addr))
		أرجع it->دومی.استخراج(range_جابه_جایی(it, addr), داده_عرض_2);
	وإلا
		أرجع default_مقدار;
}

MemContents::addr_t MemContents::شمار_range(addr_t begin_addr, addr_t انتها_addr) ثابت {
	addr_t شمار = 0;
	لكل(خودکار it = _range_at(begin_addr); _range_overlaps(it, begin_addr, انتها_addr); it++) {
		خودکار اولی = std::max(_range_begin(it), begin_addr);
		خودکار آخرین = std::min(range_انتها(it), انتها_addr);
		شمار += آخرین - اولی;
	}
	أرجع شمار;
}

فراغ MemContents::clear_range(addr_t begin_addr, addr_t انتها_addr) {
	إذا(begin_addr >= انتها_addr) أرجع;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	خودکار begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	خودکار انتها_it = مقادیر_2.فوقانی_bound(انتها_addr - 1);
	إذا(begin_it == انتها_it)
		أرجع; // nothing to do
	// the last iterator affected is one before the first one not affected
	خودکار آخرین_it = std::بعدی(انتها_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	إذا(range_شامل_است(آخرین_it, انتها_addr - 1)) {
		خودکار new_begin = انتها_addr;
		خودکار انتها = range_انتها(آخرین_it);
		// if there is data past the end address, preserve it by creating a new range
		إذا(new_begin != انتها)
			انتها_it = مقادیر_2.emplace_hint(آخرین_it, new_begin, آخرین_it->دومی.استخراج(range_جابه_جایی(آخرین_it, new_begin), (range_انتها(آخرین_it) - new_begin) * داده_عرض_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	إذا(range_شامل_است(begin_it, begin_addr)) {
		خودکار new_انتها = begin_addr;
		// if there is data before the start address, truncate but don't delete
		إذا(new_انتها != begin_it->اولی) {
			begin_it->دومی.extu(range_جابه_جایی(begin_it, new_انتها));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	مقادیر_2.erase(begin_it, انتها_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t انتها_addr) {
	إذا(begin_addr >= انتها_addr)
		أرجع مقادیر_2.انتها(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	خودکار پایینی_it = begin_addr == 0 ? مقادیر_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	إذا (range_شامل_است(پایینی_it, begin_addr, انتها_addr))
		أرجع پایینی_it;
	// find the first range containing any addr >= end_addr
	خودکار فوقانی_it = _range_at(انتها_addr);
	// check if either of the two ranges we just found touch our range
	منطقي پایینی_touch = begin_addr > 0 && range_شامل_است(پایینی_it, begin_addr - 1);
	منطقي فوقانی_touch = range_شامل_است(فوقانی_it, انتها_addr);
	إذا (پایینی_touch && فوقانی_touch) {
		log_assert (پایینی_it != فوقانی_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		خودکار فوقانی_انتها = range_انتها(فوقانی_it);
		// make range bigger (maybe reserve here instead of resize?)
		پایینی_it->دومی.resize(range_جابه_جایی(پایینی_it, فوقانی_انتها), State::Sx);
		// copy only the data beyond our range
		std::copy(range_داده(فوقانی_it, انتها_addr), range_داده(فوقانی_it, فوقانی_انتها), range_داده(پایینی_it, انتها_addr));
		// keep lower_it, but delete upper_it
		مقادیر_2.erase(std::بعدی(پایینی_it), std::بعدی(فوقانی_it));
		أرجع پایینی_it;
	} وإلا إذا (پایینی_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		پایینی_it->دومی.resize(range_جابه_جایی(پایینی_it, انتها_addr), State::Sx);
		// keep lower_it and upper_it
		مقادیر_2.erase(std::بعدی(پایینی_it), فوقانی_it);
		أرجع پایینی_it;
	} وإلا إذا (فوقانی_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const داده(State::Sx, (_range_begin(فوقانی_it) - begin_addr) * داده_عرض_2);
		داده.append(std::move(فوقانی_it->دومی));
		// delete lower_it and upper_it, then reinsert
		مقادیر_2.erase(پایینی_it, std::بعدی(فوقانی_it));
		أرجع مقادیر_2.emplace(begin_addr, std::move(داده)).اولی;
	} وإلا {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		مقادیر_2.erase(پایینی_it, فوقانی_it);
		أرجع مقادیر_2.emplace(begin_addr, RTLIL::Const(State::Sx, (انتها_addr - begin_addr) * داده_عرض_2)).اولی;
	}
}

فراغ MemContents::درج_concatenated(addr_t addr, RTLIL::Const ثابت &مقادیر) {
	addr_t واژگان_2 = (مقادیر.اندازه() + داده_عرض_2 - 1) / داده_عرض_2;
	log_assert(addr < (addr_t)(1<<addr_عرض_2));
	log_assert(واژگان_2 <= (addr_t)(1<<addr_عرض_2) - addr);
	خودکار it = _reserve_range(addr, addr + واژگان_2);
	خودکار مقصد_begin = range_داده(it, addr);
	std::copy(مقادیر.begin(), مقادیر.انتها(), مقصد_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(مقصد_begin + مقادیر.اندازه(), مقصد_begin + واژگان_2 * داده_عرض_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &کلمه) {
	خودکار from_انتها = کلمه.اندازه() <= داده_عرض_2 ? کلمه.انتها() : کلمه.begin() + داده_عرض_2;
	خودکار مقصد_انتها = std::copy(کلمه.begin(), from_انتها, it);
	خودکار it_بعدی = std::بعدی(it, داده_عرض_2);
	std::fill(مقصد_انتها, it_بعدی, State::S0);
	أرجع it_بعدی;
}
