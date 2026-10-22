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

#অন্তর্ভাব "kernel/mem.h"
#অন্তর্ভাব "kernel/ff.h"

USING_YOSYS_NAMESPACE

শূন্য Mem::remove() {
	যদি (cell) {
		module->remove(cell);
		cell = nullptr;
	}
	যদি (mem) {
		module->memories.erase(mem->name);
		delete mem;
		mem = nullptr;
	}
	জন্য (auto &port : rd_ports) {
		যদি (port.cell) {
			module->remove(port.cell);
			port.cell = nullptr;
		}
	}
	জন্য (auto &port : wr_ports) {
		যদি (port.cell) {
			module->remove(port.cell);
			port.cell = nullptr;
		}
	}
	জন্য (auto &init : inits) {
		যদি (init.cell) {
			module->remove(init.cell);
			init.cell = nullptr;
		}
	}
}

শূন্য Mem::emit() {
	check();
	std::vector<পূর্ণসংখ্যা> rd_বাম;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++) {
		auto &port = rd_ports[i];
		যদি (port.removed) {
			যদি (port.cell) {
				module->remove(port.cell);
			}
		} অন্যথা {
			rd_বাম.ঠেলো_back(i);
		}
	}
	std::vector<পূর্ণসংখ্যা> wr_বাম;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
		auto &port = wr_ports[i];
		যদি (port.removed) {
			যদি (port.cell) {
				module->remove(port.cell);
			}
		} অন্যথা {
			wr_বাম.ঠেলো_back(i);
		}
	}
	std::vector<পূর্ণসংখ্যা> init_বাম;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(inits); i++) {
		auto &init = inits[i];
		যদি (init.removed) {
			যদি (init.cell) {
				module->remove(init.cell);
			}
		} অন্যথা {
			init_বাম.ঠেলো_back(i);
		}
	}
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_বাম); i++)
		যদি (i != rd_বাম[i])
			std::swap(rd_ports[i], rd_ports[rd_বাম[i]]);
	rd_ports.resize(Get_আকার(rd_বাম));
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_বাম); i++)
		যদি (i != wr_বাম[i])
			std::swap(wr_ports[i], wr_ports[wr_বাম[i]]);
	wr_ports.resize(Get_আকার(wr_বাম));
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(init_বাম); i++)
		যদি (i != init_বাম[i])
			std::swap(inits[i], inits[init_বাম[i]]);
	inits.resize(Get_আকার(init_বাম));

	জন্য (auto &port : rd_ports) {
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_বাম); i++) {
			port.transparency_mask[i] = port.transparency_mask[wr_বাম[i]];
			port.collision_x_mask[i] = port.collision_x_mask[wr_বাম[i]];
		}
		port.transparency_mask.resize(Get_আকার(wr_বাম));
		port.collision_x_mask.resize(Get_আকার(wr_বাম));
	}
	জন্য (auto &port : wr_ports) {
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_বাম); i++)
			port.priority_mask[i] = port.priority_mask[wr_বাম[i]];
		port.priority_mask.resize(Get_আকার(wr_বাম));
	}

	যদি (packed) {
		যদি (mem) {
			module->memories.erase(mem->name);
			delete mem;
			mem = nullptr;
		}
		যদি (!cell) {
			যদি (memid.empty())
				memid = PooledName(module->design, module->twines().add(NEW_ID));
			cell = module->addCell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->attributes = attributes;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::WIDTH] = Const(width);
		cell->parameters[ID::OFFSET] = Const(start_offset);
		cell->parameters[ID::আকার_3] = Const(আকার_2);
		SigSpec rd_clk, rd_en, rd_addr, rd_তথ্য;
		SigSpec wr_clk, wr_en, wr_addr, wr_তথ্য;
		SigSpec rd_arst, rd_srst;
		পূর্ণসংখ্যা abits = 0;
		জন্য (auto &port : rd_ports)
			abits = std::max(abits, Get_আকার(port.addr));
		জন্য (auto &port : wr_ports)
			abits = std::max(abits, Get_আকার(port.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::vector<পূর্ণসংখ্যা> wr_port_xlat;
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++)
			জন্য (পূর্ণসংখ্যা j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_port_xlat.ঠেলো_back(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_mask_builder;
		Const::Builder rd_collision_x_mask_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_মান_builder;
		Const::Builder rd_srst_মান_builder;
		Const::Builder rd_init_মান_builder;
		জন্য (auto &port : rd_ports) {
			জন্য (auto attr: port.attributes)
				যদি (!cell->has_attribute(attr.first))
					cell->attributes.ঢোকাও(attr);
			যদি (port.cell) {
				module->remove(port.cell);
				port.cell = nullptr;
			}
			জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << port.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ঠেলো_back(State(sub != 0));
				rd_clk_enable_builder.ঠেলো_back(State(port.clk_enable));
				rd_clk_polarity_builder.ঠেলো_back(State(port.clk_polarity));
				rd_ce_over_srst_builder.ঠেলো_back(State(port.ce_over_srst));
				rd_clk.append(port.clk);
				rd_arst.append(port.arst);
				rd_srst.append(port.srst);
				rd_en.append(port.en);
				SigSpec addr = port.sub_addr(sub);
				addr.extend_u0(abits, মিথ্যা);
				rd_addr.append(addr);
				log_assert(Get_আকার(addr) == abits);
				জন্য (auto idx : wr_port_xlat) {
					rd_transparency_mask_builder.ঠেলো_back(State(যৌক্তিক(port.transparency_mask[idx])));
					rd_collision_x_mask_builder.ঠেলো_back(State(যৌক্তিক(port.collision_x_mask[idx])));
				}
			}
			rd_তথ্য.append(port.তথ্য);
			জন্য (auto bit : port.arst_মান)
				rd_arst_মান_builder.ঠেলো_back(bit);
			জন্য (auto bit : port.srst_মান)
				rd_srst_মান_builder.ঠেলো_back(bit);
			জন্য (auto bit : port.init_মান)
				rd_init_মান_builder.ঠেলো_back(bit);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_mask = rd_transparency_mask_builder.build();
		Const rd_collision_x_mask = rd_collision_x_mask_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_মান = rd_arst_মান_builder.build();
		Const rd_srst_মান = rd_srst_মান_builder.build();
		Const rd_init_মান = rd_init_মান_builder.build();
		যদি (rd_ports.empty()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_মান = State::S0;
			rd_srst_মান = State::S0;
			rd_init_মান = State::S0;
		}
		যদি (rd_ports.empty() || wr_ports.empty()) {
			rd_transparency_mask = State::S0;
			rd_collision_x_mask = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_আকার(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_MASK] = rd_transparency_mask;
		cell->parameters[ID::RD_COLLISION_X_MASK] = rd_collision_x_mask;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_মান] = rd_arst_মান;
		cell->parameters[ID::RD_SRST_মান] = rd_srst_মান;
		cell->parameters[ID::RD_INIT_মান] = rd_init_মান;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->setPort(ID::RD_CLK, rd_clk);
		cell->setPort(ID::RD_EN, rd_en);
		cell->setPort(ID::RD_ARST, rd_arst);
		cell->setPort(ID::RD_SRST, rd_srst);
		cell->setPort(ID::RD_ADDR, rd_addr);
		cell->setPort(ID::RD_তথ্য, rd_তথ্য);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_priority_mask_builder;
		জন্য (auto &port : wr_ports) {
			জন্য (auto attr: port.attributes)
				যদি (!cell->has_attribute(attr.first))
					cell->attributes.ঢোকাও(attr);
			যদি (port.cell) {
				module->remove(port.cell);
				port.cell = nullptr;
			}
			জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << port.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ঠেলো_back(State(sub != 0));
				wr_clk_enable_builder.ঠেলো_back(State(port.clk_enable));
				wr_clk_polarity_builder.ঠেলো_back(State(port.clk_polarity));
				wr_clk.append(port.clk);
				জন্য (auto idx : wr_port_xlat)
					wr_priority_mask_builder.ঠেলো_back(State(যৌক্তিক(port.priority_mask[idx])));
				SigSpec addr = port.sub_addr(sub);
				addr.extend_u0(abits, মিথ্যা);
				wr_addr.append(addr);
				log_assert(Get_আকার(addr) == abits);
			}
			wr_en.append(port.en);
			wr_তথ্য.append(port.তথ্য);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_priority_mask = wr_priority_mask_builder.build();
		যদি (wr_ports.empty()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_priority_mask = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_আকার(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_PRIORITY_MASK] = wr_priority_mask;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->setPort(ID::WR_CLK, wr_clk);
		cell->setPort(ID::WR_EN, wr_en);
		cell->setPort(ID::WR_ADDR, wr_addr);
		cell->setPort(ID::WR_তথ্য, wr_তথ্য);
		জন্য (auto &init : inits) {
			জন্য (auto attr: init.attributes)
				যদি (!cell->has_attribute(attr.first))
					cell->attributes.ঢোকাও(attr);
			যদি (init.cell) {
				module->remove(init.cell);
				init.cell = nullptr;
			}
		}
		cell->parameters[ID::INIT] = get_init_তথ্য();
	} অন্যথা {
		যদি (cell) {
			module->remove(cell);
			cell = nullptr;
		}
		যদি (!mem) {
			যদি (memid.empty())
				memid = PooledName(module->design, module->twines().add(NEW_ID));
			mem = module->addMemory(memid);
		}
		mem->width = width;
		mem->start_offset = start_offset;
		mem->আকার_2 = আকার_2;
		mem->attributes = attributes;
		জন্য (auto &port : rd_ports) {
			যদি (!port.cell)
				port.cell = module->addCell(NEW_ID, ID($memrd_v2));
			port.cell->type = ID($memrd_v2);
			port.cell->attributes = port.attributes;
			port.cell->parameters[ID::MEMID] = memid.str();
			port.cell->parameters[ID::ABITS] = Get_আকার(port.addr);
			port.cell->parameters[ID::WIDTH] = width << port.wide_log2;
			port.cell->parameters[ID::CLK_ENABLE] = port.clk_enable;
			port.cell->parameters[ID::CLK_POLARITY] = port.clk_polarity;
			port.cell->parameters[ID::CE_OVER_SRST] = port.ce_over_srst;
			port.cell->parameters[ID::ARST_মান] = port.arst_মান;
			port.cell->parameters[ID::SRST_মান] = port.srst_মান;
			port.cell->parameters[ID::INIT_মান] = port.init_মান;
			port.cell->parameters[ID::TRANSPARENCY_MASK] = port.transparency_mask;
			port.cell->parameters[ID::COLLISION_X_MASK] = port.collision_x_mask;
			port.cell->parameters.erase(ID::TRANSPARENT);
			port.cell->setPort(ID::CLK, port.clk);
			port.cell->setPort(ID::EN, port.en);
			port.cell->setPort(ID::ARST, port.arst);
			port.cell->setPort(ID::SRST, port.srst);
			port.cell->setPort(ID::ADDR, port.addr);
			port.cell->setPort(ID::তথ্য_2, port.তথ্য);
		}
		পূর্ণসংখ্যা idx = 0;
		জন্য (auto &port : wr_ports) {
			যদি (!port.cell)
				port.cell = module->addCell(NEW_ID, ID($memwr_v2));
			port.cell->type = ID($memwr_v2);
			port.cell->attributes = port.attributes;
			যদি (port.cell->parameters.গুনতি(ID::PRIORITY))
				port.cell->parameters.erase(ID::PRIORITY);
			port.cell->parameters[ID::MEMID] = memid.str();
			port.cell->parameters[ID::ABITS] = Get_আকার(port.addr);
			port.cell->parameters[ID::WIDTH] = width << port.wide_log2;
			port.cell->parameters[ID::CLK_ENABLE] = port.clk_enable;
			port.cell->parameters[ID::CLK_POLARITY] = port.clk_polarity;
			port.cell->parameters[ID::PORTID] = idx++;
			port.cell->parameters[ID::PRIORITY_MASK] = port.priority_mask;
			port.cell->setPort(ID::CLK, port.clk);
			port.cell->setPort(ID::EN, port.en);
			port.cell->setPort(ID::ADDR, port.addr);
			port.cell->setPort(ID::তথ্য_2, port.তথ্য);
		}
		idx = 0;
		জন্য (auto &init : inits) {
			যৌক্তিক v2 = !init.en.is_fully_ones();
			যদি (!init.cell)
				init.cell = module->addCell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			অন্যথা {
				যদি (!v2)
					init.cell->unsetPort(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->attributes = init.attributes;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_আকার(init.addr);
			init.cell->parameters[ID::WIDTH] = width;
			init.cell->parameters[ID::শব্দগুলি] = Get_আকার(init.তথ্য) / width;
			init.cell->parameters[ID::PRIORITY] = idx++;
			init.cell->setPort(ID::ADDR, init.addr);
			init.cell->setPort(ID::তথ্য_2, init.তথ্য);
			যদি (v2)
				init.cell->setPort(ID::EN, init.en);
		}
	}
}

শূন্য Mem::clear_inits() {
	জন্য (auto &init : inits)
		init.removed = সত্য;
}

শূন্য Mem::coalesce_inits() {
	// start address -> end address
	std::map<পূর্ণসংখ্যা, পূর্ণসংখ্যা> chunks;
	// Figure out chunk boundaries.
	জন্য (auto &init : inits) {
		যদি (init.removed)
			চালু;
		যৌক্তিক valid = মিথ্যা;
		জন্য (auto bit : init.en)
			যদি (bit == State::S1)
				valid = সত্য;
		যদি (!valid) {
			init.removed = সত্য;
			চালু;
		}
		পূর্ণসংখ্যা addr = init.addr.as_int();
		পূর্ণসংখ্যা addr_e = addr + Get_আকার(init.তথ্য) / width;
		auto it_e = chunks.upper_bound(addr_e);
		auto it = it_e;
		যখন (it != chunks.begin()) {
			--it;
			যদি (it->second < addr) {
				++it;
				বিরতি;
			}
		}
		যদি (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} অন্যথা {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			যদি (it->first < addr)
				addr = it->first;
			auto it_last = it_e;
			it_last--;
			যদি (it_last->second > addr_e)
				addr_e = it_last->second;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<পূর্ণসংখ্যা, std::vector<পূর্ণসংখ্যা>> inits_by_chunk;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(inits); i++) {
		auto &init = inits[i];
		যদি (init.removed)
			চালু;
		auto it = chunks.upper_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->first].ঠেলো_back(i);
		পূর্ণসংখ্যা addr = init.addr.as_int();
		পূর্ণসংখ্যা addr_e = addr + Get_আকার(init.তথ্য) / width;
		log_assert(addr >= it->first && addr_e <= it->second);
	}
	// Process each chunk.
	জন্য (auto &it : inits_by_chunk) {
		পূর্ণসংখ্যা caddr = it.first;
		পূর্ণসংখ্যা caddr_e = chunks[caddr];
		auto &chunk_inits = it.second;
		যদি (Get_আকার(chunk_inits) == 1) {
			auto &init = inits[chunk_inits[0]];
			যদি (!init.en.is_fully_ones()) {
				জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(init.তথ্য); i++)
					যদি (init.en[i % width] != State::S1)
						init.তথ্য.set(i, State::Sx);
				init.en = Const(State::S1, width);
			}
			চালু;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * width);
		জন্য (পূর্ণসংখ্যা idx : chunk_inits) {
			auto &init = inits[idx];
			পূর্ণসংখ্যা offset = (init.addr.as_int() - caddr) * width;
			log_assert(offset >= 0);
			log_assert(offset + Get_আকার(init.তথ্য) <= Get_আকার(cdata));
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(init.তথ্য); i++)
				যদি (init.en[i % width] == State::S1)
					cdata.set(i+offset, init.তথ্য[i]);
			init.removed = সত্য;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.তথ্য = cdata;
		new_init.en = Const(State::S1, width);
		inits.ঠেলো_back(new_init);
	}
}

Const Mem::get_init_তথ্য() ধ্রুবক {
	Const init_তথ্য(State::Sx, width * আকার_2);
	জন্য (auto &init : inits) {
		যদি (init.removed)
			চালু;
		পূর্ণসংখ্যা offset = (init.addr.as_int() - start_offset) * width;
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(init.তথ্য); i++)
			যদি (0 <= i+offset && i+offset < Get_আকার(init_তথ্য) && init.en[i % width] == State::S1)
				init_তথ্য.set(i+offset, init.তথ্য[i]);
	}
	ফেরত init_তথ্য;
}

শূন্য Mem::check() {
	পূর্ণসংখ্যা max_wide_log2 = 0;
	জন্য (auto &port : rd_ports) {
		যদি (port.removed)
			চালু;
		log_assert(Get_আকার(port.clk) == 1);
		log_assert(Get_আকার(port.en) == 1);
		log_assert(Get_আকার(port.arst) == 1);
		log_assert(Get_আকার(port.srst) == 1);
		log_assert(Get_আকার(port.addr) >= port.wide_log2);
		log_assert(Get_আকার(port.তথ্য) == (width << port.wide_log2));
		log_assert(Get_আকার(port.init_মান) == (width << port.wide_log2));
		log_assert(Get_আকার(port.arst_মান) == (width << port.wide_log2));
		log_assert(Get_আকার(port.srst_মান) == (width << port.wide_log2));
		যদি (!port.clk_enable) {
			log_assert(port.en == State::S1);
			log_assert(port.arst == State::S0);
			log_assert(port.srst == State::S0);
		}
		জন্য (পূর্ণসংখ্যা j = 0; j < port.wide_log2; j++) {
			log_assert(port.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, port.wide_log2);
		log_assert(Get_আকার(port.transparency_mask) == Get_আকার(wr_ports));
		log_assert(Get_আকার(port.collision_x_mask) == Get_আকার(wr_ports));
		জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(wr_ports); j++) {
			auto &wport = wr_ports[j];
			যদি ((port.transparency_mask[j] || port.collision_x_mask[j]) && !wport.removed) {
				log_assert(port.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(port.clk == wport.clk);
				log_assert(port.clk_polarity == wport.clk_polarity);
			}
			log_assert(!port.transparency_mask[j] || !port.collision_x_mask[j]);
		}
	}
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
		auto &port = wr_ports[i];
		যদি (port.removed)
			চালু;
		log_assert(Get_আকার(port.clk) == 1);
		log_assert(Get_আকার(port.en) == (width << port.wide_log2));
		log_assert(Get_আকার(port.তথ্য) == (width << port.wide_log2));
		log_assert(Get_আকার(port.addr) >= port.wide_log2);
		জন্য (পূর্ণসংখ্যা j = 0; j < port.wide_log2; j++) {
			log_assert(port.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, port.wide_log2);
		log_assert(Get_আকার(port.priority_mask) == Get_আকার(wr_ports));
		জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(wr_ports); j++) {
			auto &wport = wr_ports[j];
			যদি (port.priority_mask[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(port.clk_enable == wport.clk_enable);
				যদি (port.clk_enable) {
					log_assert(port.clk == wport.clk);
					log_assert(port.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	পূর্ণসংখ্যা mask = (1 << max_wide_log2) - 1;
	log_assert(!(start_offset & mask));
	log_assert(!(আকার_2 & mask));
	log_assert(width != 0);
}

নামাকাশ {

	কাঠামো Mem_সূচক {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_সূচক (Module *module) {
			জন্য (auto cell: module->cells()) {
				যদি (cell->type.in(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].ঢোকাও(cell);
				অন্যথা যদি (cell->type.in(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].ঢোকাও(cell);
				অন্যথা যদি (cell->type.in(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].ঢোকাও(cell);
			}
		}
	};

	Mem mem_from_memory(Module *module, RTLIL::Memory *mem, ধ্রুবক Mem_সূচক &সূচক) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->width, mem->start_offset, mem->আকার_2);
		res.packed = মিথ্যা;
		res.mem = mem;
		res.attributes = mem->attributes;
		std::vector<যৌক্তিক> rd_transparent;
		std::vector<পূর্ণসংখ্যা> wr_portid;
		যদি (সূচক.rd_ports.গুনতি(memid)) {
			জন্য (auto cell : সূচক.rd_ports.at(memid)) {
				MemRd mrd;
				যৌক্তিক is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.attributes = cell->attributes;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->getPort(ID::CLK);
				mrd.en = cell->getPort(ID::EN);
				mrd.addr = cell->getPort(ID::ADDR);
				mrd.তথ্য = cell->getPort(ID::তথ্য_2);
				mrd.wide_log2 = ceil_log2(Get_আকার(mrd.তথ্য) / mem->width);
				যৌক্তিক transparent = মিথ্যা;
				যদি (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = মিথ্যা;
					mrd.arst_মান = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.srst_মান = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.init_মান = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					যদি (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = মিথ্যা;
						যদি (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} অন্যথা {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_মান = cell->parameters.at(ID::ARST_মান);
					mrd.srst_মান = cell->parameters.at(ID::SRST_মান);
					mrd.init_মান = cell->parameters.at(ID::INIT_মান);
					mrd.arst = cell->getPort(ID::ARST);
					mrd.srst = cell->getPort(ID::SRST);
				}
				res.rd_ports.ঠেলো_back(mrd);
				rd_transparent.ঠেলো_back(transparent);
			}
		}
		যদি (সূচক.wr_ports.গুনতি(memid)) {
			std::vector<std::pair<পূর্ণসংখ্যা, MemWr>> ports;
			জন্য (auto cell : সূচক.wr_ports.at(memid)) {
				MemWr mwr;
				যৌক্তিক is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.attributes = cell->attributes;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->getPort(ID::CLK);
				mwr.en = cell->getPort(ID::EN);
				mwr.addr = cell->getPort(ID::ADDR);
				mwr.তথ্য = cell->getPort(ID::তথ্য_2);
				mwr.wide_log2 = ceil_log2(Get_আকার(mwr.তথ্য) / mem->width);
				ports.ঠেলো_back(std::make_pair(cell->parameters.at(is_compat ? ID::PRIORITY : ID::PORTID).as_int(), mwr));
			}
			std::sort(ports.begin(), ports.end(), [](ধ্রুবক std::pair<পূর্ণসংখ্যা, MemWr> &a, ধ্রুবক std::pair<পূর্ণসংখ্যা, MemWr> &b) { ফেরত a.first < b.first; });
			জন্য (auto &it : ports) {
				res.wr_ports.ঠেলো_back(it.second);
				wr_portid.ঠেলো_back(it.first);
			}
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(res.wr_ports); i++) {
				auto &port = res.wr_ports[i];
				যৌক্তিক is_compat = port.cell->type == ID($memwr);
				যদি (is_compat) {
					port.priority_mask.resize(Get_আকার(res.wr_ports));
					জন্য (পূর্ণসংখ্যা j = 0; j < i; j++) {
						auto &oport = res.wr_ports[j];
						যদি (port.clk_enable != oport.clk_enable)
							চালু;
						যদি (port.clk_enable && port.clk != oport.clk)
							চালু;
						যদি (port.clk_enable && port.clk_polarity != oport.clk_polarity)
							চালু;
						port.priority_mask[j] = সত্য;
					}
				} অন্যথা {
					Const orig_prio_mask = port.cell->parameters.at(ID::PRIORITY_MASK);
					জন্য (পূর্ণসংখ্যা orig_portid : wr_portid) {
						যৌক্তিক has_prio = orig_portid < Get_আকার(orig_prio_mask) && orig_prio_mask[orig_portid] == State::S1;
						port.priority_mask.ঠেলো_back(has_prio);
					}
				}
			}
		}
		যদি (সূচক.inits.গুনতি(memid)) {
			std::vector<std::pair<পূর্ণসংখ্যা, MemInit>> inits;
			জন্য (auto cell : সূচক.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.attributes = cell->attributes;
				auto addr = cell->getPort(ID::ADDR);
				auto তথ্য = cell->getPort(ID::তথ্য_2);
				যদি (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				যদি (!তথ্য.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(তথ্য), cell);
				init.addr = addr.as_const();
				init.তথ্য = তথ্য.as_const();
				যদি (cell->type == ID($meminit_v2)) {
					auto en = cell->getPort(ID::EN);
					যদি (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} অন্যথা {
					init.en = RTLIL::Const(State::S1, mem->width);
				}
				inits.ঠেলো_back(std::make_pair(cell->parameters.at(ID::PRIORITY).as_int(), init));
			}
			std::sort(inits.begin(), inits.end(), [](ধ্রুবক std::pair<পূর্ণসংখ্যা, MemInit> &a, ধ্রুবক std::pair<পূর্ণসংখ্যা, MemInit> &b) { ফেরত a.first < b.first; });
			জন্য (auto &it : inits)
				res.inits.ঠেলো_back(it.second);
		}
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(res.rd_ports); i++) {
			auto &port = res.rd_ports[i];
			যৌক্তিক is_compat = port.cell->type == ID($memrd);
			যদি (is_compat) {
				port.transparency_mask.resize(Get_আকার(res.wr_ports));
				port.collision_x_mask.resize(Get_আকার(res.wr_ports));
				যদি (!rd_transparent[i])
					চালু;
				যদি (!port.clk_enable)
					চালু;
				জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(res.wr_ports); j++) {
					auto &wport = res.wr_ports[j];
					যদি (!wport.clk_enable)
						চালু;
					যদি (port.clk != wport.clk)
						চালু;
					যদি (port.clk_polarity != wport.clk_polarity)
						চালু;
					port.transparency_mask[j] = সত্য;
				}
			} অন্যথা {
				Const orig_trans_mask = port.cell->parameters.at(ID::TRANSPARENCY_MASK);
				Const orig_cx_mask = port.cell->parameters.at(ID::COLLISION_X_MASK);
				জন্য (পূর্ণসংখ্যা orig_portid : wr_portid) {
					port.transparency_mask.ঠেলো_back(orig_portid < Get_আকার(orig_trans_mask) && orig_trans_mask[orig_portid] == State::S1);
					port.collision_x_mask.ঠেলো_back(orig_portid < Get_আকার(orig_cx_mask) && orig_cx_mask[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		ফেরত res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().add(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::WIDTH).as_int(),
			cell->parameters.at(ID::OFFSET).as_int(),
			cell->parameters.at(ID::আকার_3).as_int()
		);
		যৌক্তিক is_compat = cell->type == ID($mem);
		পূর্ণসংখ্যা abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = সত্য;
		res.cell = cell;
		res.attributes = cell->attributes;
		Const &init = cell->parameters.at(ID::INIT);
		যদি (!init.is_fully_undef()) {
			পূর্ণসংখ্যা pos = 0;
			যখন (pos < res.আকার_2) {
				Const word = init.extract(pos * res.width, res.width, State::Sx);
				যদি (word.is_fully_undef()) {
					pos++;
				} অন্যথা {
					পূর্ণসংখ্যা epos;
					জন্য (epos = pos; epos < res.আকার_2; epos++) {
						Const eword = init.extract(epos * res.width, res.width, State::Sx);
						যদি (eword.is_fully_undef())
							বিরতি;
					}
					MemInit minit;
					minit.addr = res.start_offset + pos;
					minit.তথ্য = init.extract(pos * res.width, (epos - pos) * res.width, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.width);
					res.inits.ঠেলো_back(minit);
					pos = epos;
				}
			}
		}
		পূর্ণসংখ্যা n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		পূর্ণসংখ্যা n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		জন্য (পূর্ণসংখ্যা i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			যখন (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).extract(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).extract(i, 1).as_bool();
			mrd.clk = cell->getPort(ID::RD_CLK).extract(i, 1);
			mrd.en = cell->getPort(ID::RD_EN).extract(i, 1);
			mrd.addr = cell->getPort(ID::RD_ADDR).extract(i * abits, abits);
			mrd.তথ্য = cell->getPort(ID::RD_তথ্য).extract(i * res.width, (ni - i) * res.width);
			যদি (is_compat) {
				mrd.ce_over_srst = মিথ্যা;
				mrd.arst_মান = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.srst_মান = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.init_মান = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} অন্যথা {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).extract(i, 1).as_bool();
				mrd.arst_মান = cell->parameters.at(ID::RD_ARST_মান).extract(i * res.width, (ni - i) * res.width);
				mrd.srst_মান = cell->parameters.at(ID::RD_SRST_মান).extract(i * res.width, (ni - i) * res.width);
				mrd.init_মান = cell->parameters.at(ID::RD_INIT_মান).extract(i * res.width, (ni - i) * res.width);
				mrd.arst = cell->getPort(ID::RD_ARST).extract(i, 1);
				mrd.srst = cell->getPort(ID::RD_SRST).extract(i, 1);
			}
			যদি (!is_compat) {
				Const transparency_mask = cell->parameters.at(ID::RD_TRANSPARENCY_MASK).extract(i * n_wr_ports, n_wr_ports);
				Const collision_x_mask = cell->parameters.at(ID::RD_COLLISION_X_MASK).extract(i * n_wr_ports, n_wr_ports);
				জন্য (পূর্ণসংখ্যা j = 0; j < n_wr_ports; j++)
					যদি (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_mask.ঠেলো_back(transparency_mask[j] == State::S1);
						mrd.collision_x_mask.ঠেলো_back(collision_x_mask[j] == State::S1);
					}
			}
			res.rd_ports.ঠেলো_back(mrd);
		}
		জন্য (পূর্ণসংখ্যা i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			যখন (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).extract(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).extract(i, 1).as_bool();
			mwr.clk = cell->getPort(ID::WR_CLK).extract(i, 1);
			mwr.en = cell->getPort(ID::WR_EN).extract(i * res.width, (ni - i) * res.width);
			mwr.addr = cell->getPort(ID::WR_ADDR).extract(i * abits, abits);
			mwr.তথ্য = cell->getPort(ID::WR_তথ্য).extract(i * res.width, (ni - i) * res.width);
			যদি (!is_compat) {
				Const priority_mask = cell->parameters.at(ID::WR_PRIORITY_MASK).extract(i * n_wr_ports, n_wr_ports);
				জন্য (পূর্ণসংখ্যা j = 0; j < n_wr_ports; j++)
					যদি (wr_wide_continuation[j] != State::S1)
						mwr.priority_mask.ঠেলো_back(priority_mask[j] == State::S1);
			}
			res.wr_ports.ঠেলো_back(mwr);
		}
		যদি (is_compat) {
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(res.wr_ports); i++) {
				auto &port = res.wr_ports[i];
				port.priority_mask.resize(Get_আকার(res.wr_ports));
				জন্য (পূর্ণসংখ্যা j = 0; j < i; j++) {
					auto &oport = res.wr_ports[j];
					যদি (port.clk_enable != oport.clk_enable)
						চালু;
					যদি (port.clk_enable && port.clk != oport.clk)
						চালু;
					যদি (port.clk_enable && port.clk_polarity != oport.clk_polarity)
						চালু;
					port.priority_mask[j] = সত্য;
				}
			}
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(res.rd_ports); i++) {
				auto &port = res.rd_ports[i];
				port.transparency_mask.resize(Get_আকার(res.wr_ports));
				port.collision_x_mask.resize(Get_আকার(res.wr_ports));
				যদি (!cell->parameters.at(ID::RD_TRANSPARENT).extract(i, 1).as_bool())
					চালু;
				যদি (!port.clk_enable)
					চালু;
				জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(res.wr_ports); j++) {
					auto &wport = res.wr_ports[j];
					যদি (!wport.clk_enable)
						চালু;
					যদি (port.clk != wport.clk)
						চালু;
					যদি (port.clk_polarity != wport.clk_polarity)
						চালু;
					port.transparency_mask[j] = সত্য;
				}
			}
		}
		res.check();
		ফেরত res;
	}

}

std::vector<Mem> Mem::get_all_memories(Module *module) {
	std::vector<Mem> res;
	Mem_সূচক সূচক(module);
	জন্য (auto it: module->memories) {
		res.ঠেলো_back(mem_from_memory(module, it.second, সূচক));
	}
	জন্য (auto cell: module->cells()) {
		যদি (cell->type.in(ID($mem), ID($mem_v2)))
			res.ঠেলো_back(mem_from_cell(cell));
	}
	ফেরত res;
}

std::vector<Mem> Mem::get_selected_memories(Module *module) {
	std::vector<Mem> res;
	Mem_সূচক সূচক(module);
	জন্য (auto it: module->memories) {
		যদি (module->design->selected(module, it.second))
			res.ঠেলো_back(mem_from_memory(module, it.second, সূচক));
	}
	জন্য (auto cell: module->selected_cells()) {
		যদি (cell->type.in(ID($mem), ID($mem_v2)))
			res.ঠেলো_back(mem_from_cell(cell));
	}
	ফেরত res;
}

Cell *Mem::extract_rdff(পূর্ণসংখ্যা idx, FfInitVals *initvals) {
	MemRd &port = rd_ports[idx];

	যদি (!port.clk_enable)
		ফেরত nullptr;

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
	যৌক্তিক trans_use_addr = সত্য;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++)
		যদি (!port.transparency_mask[i] && !wr_ports[i].removed)
			trans_use_addr = মিথ্যা;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	যদি (Get_আকার(wr_ports) == 0)
		trans_use_addr = মিথ্যা;

	যদি (port.en != State::S1 || port.srst != State::S0 || port.arst != State::S0 || !port.init_মান.is_fully_undef())
		trans_use_addr = মিথ্যা;

	যদি (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		পূর্ণসংখ্যা width = 0;
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(port.addr); i++)
			যদি (port.addr[i].wire)
				width++;

		যদি (width)
		{
			SigSpec sig_q = module->addWire(stringf("$%s$rdreg[%d]$q", memid_str, idx), width);
			SigSpec sig_d;

			পূর্ণসংখ্যা pos = 0;
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(port.addr); i++)
				যদি (port.addr[i].wire) {
					sig_d.append(port.addr[i]);
					port.addr[i] = sig_q[pos++];
				}

			c = module->addDff(stringf("$%s$rdreg[%d]", memid_str, idx), port.clk, sig_d, sig_q, port.clk_polarity);
		} অন্যথা {
			c = nullptr;
		}
	}
	অন্যথা
	{
		log_assert(port.arst == State::S0 || port.srst == State::S0);

		SigSpec async_d = module->addWire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_আকার(port.তথ্য));
		SigSpec sig_d = async_d;

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
			auto &wport = wr_ports[i];
			যদি (wport.removed)
				চালু;
			যদি (port.transparency_mask[i] || port.collision_x_mask[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == port.clk);
				log_assert(wport.clk_enable == port.clk_enable);
				পূর্ণসংখ্যা min_wide_log2 = std::min(port.wide_log2, wport.wide_log2);
				পূর্ণসংখ্যা max_wide_log2 = std::max(port.wide_log2, wport.wide_log2);
				যৌক্তিক wide_write = wport.wide_log2 > port.wide_log2;
				জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = port.addr;
					SigSpec waddr = wport.addr;
					যদি (wide_write)
						waddr = wport.sub_addr(sub);
					অন্যথা
						raddr = port.sub_addr(sub);
					SigSpec addr_eq;
					যদি (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					পূর্ণসংখ্যা pos = 0;
					পূর্ণসংখ্যা ewidth = width << min_wide_log2;
					পূর্ণসংখ্যা wsub = wide_write ? sub : 0;
					পূর্ণসংখ্যা rsub = wide_write ? 0 : sub;
					যখন (pos < ewidth) {
						পূর্ণসংখ্যা epos = pos;
						যখন (epos < ewidth && wport.en[epos + wsub * width] == wport.en[pos + wsub * width])
							epos++;
						SigSpec cur = sig_d.extract(pos + rsub * width, epos-pos);
						SigSpec other = port.transparency_mask[i] ? wport.তথ্য.extract(pos + wsub * width, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						যদি (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * width], addr_eq);
						অন্যথা
							cond = wport.en[pos + wsub * width];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * width, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().add(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_তথ্য ff(module, initvals, name);
		ff.width = Get_আকার(port.তথ্য);
		ff.has_clk = সত্য;
		ff.sig_clk = port.clk;
		ff.pol_clk = port.clk_polarity;
		যদি (port.en != State::S1) {
			ff.has_ce = সত্য;
			ff.pol_ce = সত্য;
			ff.sig_ce = port.en;
		}
		যদি (port.arst != State::S0) {
			ff.has_arst = সত্য;
			ff.pol_arst = সত্য;
			ff.sig_arst = port.arst;
			ff.val_arst = port.arst_মান;
		}
		যদি (port.srst != State::S0) {
			ff.has_srst = সত্য;
			ff.pol_srst = সত্য;
			ff.sig_srst = port.srst;
			ff.val_srst = port.srst_মান;
			ff.ce_over_srst = ff.has_ce && port.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = port.তথ্য;
		ff.val_init = port.init_মান;
		port.তথ্য = async_d;
		c = ff.emit();
	}

	যদি (c)
		log("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	port.en = State::S1;
	port.clk = State::S0;
	port.arst = State::S0;
	port.srst = State::S0;
	port.clk_enable = মিথ্যা;
	port.clk_polarity = সত্য;
	port.ce_over_srst = মিথ্যা;
	port.arst_মান = Const(State::Sx, Get_আকার(port.তথ্য));
	port.srst_মান = Const(State::Sx, Get_আকার(port.তথ্য));
	port.init_মান = Const(State::Sx, Get_আকার(port.তথ্য));

	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
		port.transparency_mask[i] = মিথ্যা;
		port.collision_x_mask[i] = মিথ্যা;
	}

	ফেরত c;
}

শূন্য Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::vector<MemRd> new_rd_ports;
	std::vector<MemWr> new_wr_ports;
	std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> new_rd_map;
	std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> new_wr_map;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++) {
		auto &port = rd_ports[i];
		জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << port.wide_log2); sub++) {
			new_rd_map.ঠেলো_back(std::make_pair(i, sub));
		}
	}
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
		auto &port = wr_ports[i];
		জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << port.wide_log2); sub++) {
			new_wr_map.ঠেলো_back(std::make_pair(i, sub));
		}
	}
	জন্য (auto &it : new_rd_map) {
		MemRd &orig = rd_ports[it.first];
		MemRd port = orig;
		যদি (it.second != 0)
			port.cell = nullptr;
		যদি (port.wide_log2) {
			port.তথ্য = port.তথ্য.extract(it.second * width, width);
			port.init_মান = port.init_মান.extract(it.second * width, width);
			port.arst_মান = port.arst_মান.extract(it.second * width, width);
			port.srst_মান = port.srst_মান.extract(it.second * width, width);
			port.addr = port.sub_addr(it.second);
			port.wide_log2 = 0;
		}
		port.transparency_mask.clear();
		port.collision_x_mask.clear();
		জন্য (auto &it2 : new_wr_map)
			port.transparency_mask.ঠেলো_back(orig.transparency_mask[it2.first]);
		জন্য (auto &it2 : new_wr_map)
			port.collision_x_mask.ঠেলো_back(orig.collision_x_mask[it2.first]);
		new_rd_ports.ঠেলো_back(port);
	}
	জন্য (auto &it : new_wr_map) {
		MemWr &orig = wr_ports[it.first];
		MemWr port = orig;
		যদি (it.second != 0)
			port.cell = nullptr;
		যদি (port.wide_log2) {
			port.তথ্য = port.তথ্য.extract(it.second * width, width);
			port.en = port.en.extract(it.second * width, width);
			port.addr = port.sub_addr(it.second);
			port.wide_log2 = 0;
		}
		port.priority_mask.clear();
		জন্য (auto &it2 : new_wr_map)
			port.priority_mask.ঠেলো_back(orig.priority_mask[it2.first]);
		new_wr_ports.ঠেলো_back(port);
	}
	std::swap(rd_ports, new_rd_ports);
	std::swap(wr_ports, new_wr_ports);
}

শূন্য Mem::emulate_priority(পূর্ণসংখ্যা idx1, পূর্ণসংখ্যা idx2, FfInitVals *initvals)
{
	auto &port1 = wr_ports[idx1];
	auto &port2 = wr_ports[idx2];
	যদি (!port2.priority_mask[idx1])
		ফেরত;
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++) {
		auto &rport = rd_ports[i];
		যদি (rport.removed)
			চালু;
		যদি (rport.transparency_mask[idx1] && !(rport.transparency_mask[idx2] || rport.collision_x_mask[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	পূর্ণসংখ্যা min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	পূর্ণসংখ্যা max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	যৌক্তিক wide1 = port1.wide_log2 > port2.wide_log2;
	জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		যদি (wide1)
			addr1 = port1.sub_addr(sub);
		অন্যথা
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		পূর্ণসংখ্যা ewidth = width << min_wide_log2;
		পূর্ণসংখ্যা sub1 = wide1 ? sub : 0;
		পূর্ণসংখ্যা sub2 = wide1 ? 0 : sub;
		dict<std::pair<SigBit, SigBit>, SigBit> cache;
		জন্য (পূর্ণসংখ্যা pos = 0; pos < ewidth; pos++) {
			SigBit &en1 = port1.en[pos + sub1 * width];
			SigBit &en2 = port2.en[pos + sub2 * width];
			std::pair<SigBit, SigBit> চাবি(en1, en2);
			যদি (cache.গুনতি(চাবি)) {
				en1 = cache[চাবি];
			} অন্যথা {
				SigBit active2 = module->And(NEW_ID, addr_eq, en2);
				SigBit nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[চাবি] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.priority_mask[idx1] = মিথ্যা;
}

শূন্য Mem::emulate_transparency(পূর্ণসংখ্যা widx, পূর্ণসংখ্যা ridx, FfInitVals *initvals) {
	auto &wport = wr_ports[widx];
	auto &rport = rd_ports[ridx];
	log_assert(rport.transparency_mask[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	জন্য (পূর্ণসংখ্যা i = Get_আকার(wr_ports) - 1; i > widx; i--) {
		যদি (wr_ports[i].removed)
			চালু;
		যদি (rport.transparency_mask[i] && wr_ports[i].priority_mask[widx])
			emulate_transparency(i, ridx, initvals);
	}
	পূর্ণসংখ্যা min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	পূর্ণসংখ্যা max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	যৌক্তিক wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->addWire(NEW_ID, Get_আকার(wport.তথ্য));
	module->addDffe(NEW_ID, rport.clk, rport.en, wport.তথ্য, wdata_q, rport.clk_polarity, সত্য);
	জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		জন্য (পূর্ণসংখ্যা j = min_wide_log2; j < max_wide_log2; j++)
			যদি (wide_write)
				waddr = wport.sub_addr(sub);
			অন্যথা
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		যদি (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		পূর্ণসংখ্যা pos = 0;
		পূর্ণসংখ্যা ewidth = width << min_wide_log2;
		পূর্ণসংখ্যা wsub = wide_write ? sub : 0;
		পূর্ণসংখ্যা rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->addWire(NEW_ID, ewidth);
		যখন (pos < ewidth) {
			পূর্ণসংখ্যা epos = pos;
			যখন (epos < ewidth && wport.en[epos + wsub * width] == wport.en[pos + wsub * width])
				epos++;
			SigSpec cond;
			যদি (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * width], addr_eq);
			অন্যথা
				cond = wport.en[pos + wsub * width];
			SigSpec cond_q = module->addWire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_তথ্য ff(module, initvals, NEW_ID);
			ff.width = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = সত্য;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			যদি (rport.en != State::S1) {
				ff.has_ce = সত্য;
				ff.sig_ce = rport.en;
				ff.pol_ce = সত্য;
			}
			যদি (rport.arst != State::S0) {
				ff.has_arst = সত্য;
				ff.sig_arst = rport.arst;
				ff.pol_arst = সত্য;
				ff.val_arst = State::S0;
			}
			যদি (rport.srst != State::S0) {
				ff.has_srst = সত্য;
				ff.sig_srst = rport.srst;
				ff.pol_srst = সত্য;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			যদি (!rport.init_মান.is_fully_undef())
				ff.val_init = State::S0;
			অন্যথা
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.extract(pos, epos-pos);
			SigSpec other = wdata_q.extract(pos + wsub * width, epos-pos);
			SigSpec dest = rport.তথ্য.extract(pos + rsub * width, epos-pos);
			module->addMux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.তথ্য.replace(rsub * width, rdata_a);
	}
	rport.transparency_mask[widx] = মিথ্যা;
	rport.collision_x_mask[widx] = সত্য;
}

শূন্য Mem::prepare_wr_merge(পূর্ণসংখ্যা idx1, পূর্ণসংখ্যা idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	auto &port1 = wr_ports[idx1];
	auto &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	জন্য (পূর্ণসংখ্যা i = 0; i < idx1; i++)
		যদি (port2.priority_mask[i])
			port1.priority_mask[i] = সত্য;
	// If port 2 has priority over a port after port 1, emulate it.
	জন্য (পূর্ণসংখ্যা i = idx1 + 1; i < idx2; i++)
		যদি (port2.priority_mask[i] && !wr_ports[i].removed)
			emulate_priority(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	জন্য (পূর্ণসংখ্যা i = idx2 + 1; i < Get_আকার(wr_ports); i++) {
		auto &oport = wr_ports[i];
		যদি (oport.priority_mask[idx2])
			oport.priority_mask[idx1] = সত্য;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++) {
		auto &rport = rd_ports[i];
		যদি (rport.removed)
			চালু;
		// If collision already undefined with both ports, it's fine.
		যদি (rport.collision_x_mask[idx1] && rport.collision_x_mask[idx2])
			চালু;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		যদি (rport.collision_x_mask[idx1]) {
			rport.collision_x_mask[idx1] = মিথ্যা;
			rport.transparency_mask[idx1] = rport.transparency_mask[idx2];
			চালু;
		}
		যদি (rport.collision_x_mask[idx2]) {
			rport.collision_x_mask[idx2] = মিথ্যা;
			rport.transparency_mask[idx2] = rport.transparency_mask[idx1];
			চালু;
		}
		// If transparent with both ports, also fine.
		যদি (rport.transparency_mask[idx1] && rport.transparency_mask[idx2])
			চালু;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		যদি (rport.transparency_mask[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_mask[idx1] = মিথ্যা;
			চালু;
		}
		যদি (rport.transparency_mask[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_mask[idx2] = মিথ্যা;
			চালু;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

শূন্য Mem::prepare_rd_merge(পূর্ণসংখ্যা idx1, পূর্ণসংখ্যা idx2, FfInitVals *initvals) {
	auto &port1 = rd_ports[idx1];
	auto &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wr_ports); i++) {
		যদি (wr_ports[i].removed)
			চালু;
		// Both ports undefined, OK.
		যদি (port1.collision_x_mask[i] && port2.collision_x_mask[i])
			চালু;
		// Only one port undefined — change its behavior
		// to align with the other port.
		যদি (port1.collision_x_mask[i]) {
			port1.collision_x_mask[i] = মিথ্যা;
			port1.transparency_mask[i] = port2.transparency_mask[i];
			চালু;
		}
		যদি (port2.collision_x_mask[i]) {
			port2.collision_x_mask[i] = মিথ্যা;
			port2.transparency_mask[i] = port1.transparency_mask[i];
			চালু;
		}
		// Both ports transparent, OK.
		যদি (port1.transparency_mask[i] && port2.transparency_mask[i])
			চালু;
		// Only one port transparent — emulate transparency
		// on the other.
		যদি (port1.transparency_mask[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_mask[i] = মিথ্যা;
			চালু;
		}
		যদি (port2.transparency_mask[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_mask[i] = মিথ্যা;
			চালু;
		}
		// No ports transparent, OK.
	}

}

শূন্য Mem::widen_prep(পূর্ণসংখ্যা wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	পূর্ণসংখ্যা mask = ((1 << wide_log2) - 1);
	পূর্ণসংখ্যা delta = start_offset & mask;
	start_offset -= delta;
	আকার_2 += delta;
	যদি (আকার_2 & mask) {
		আকার_2 |= mask;
		আকার_2++;
	}
}

শূন্য Mem::widen_wr_port(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা wide_log2) {
	widen_prep(wide_log2);
	auto &port = wr_ports[idx];
	log_assert(port.wide_log2 <= wide_log2);
	যদি (port.wide_log2 < wide_log2) {
		SigSpec new_তথ্য, new_en;
		SigSpec addr_lo = port.addr.extract(0, wide_log2);
		জন্য (পূর্ণসংখ্যা sub = 0; sub < (1 << wide_log2); sub += (1 << port.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			যদি (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_তথ্য.append(port.তথ্য);
				new_en.append(port.en);
			} অন্যথা যদি (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_তথ্য.append(Const(State::Sx, Get_আকার(port.তথ্য)));
				new_en.append(Const(State::S0, Get_আকার(port.তথ্য)));
			} অন্যথা {
				// May or may not write to this subword.
				new_তথ্য.append(port.তথ্য);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_আকার(port.তথ্য)), port.en, addr_eq);
				new_en.append(en);
			}
		}
		port.addr.replace(port.wide_log2, Const(State::S0, wide_log2 - port.wide_log2));
		port.তথ্য = new_তথ্য;
		port.en = new_en;
		port.wide_log2 = wide_log2;
	}
}

শূন্য Mem::emulate_rden(পূর্ণসংখ্যা idx, FfInitVals *initvals) {
	auto &port = rd_ports[idx];
	log_assert(port.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
	Wire *পূর্ব_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
	Wire *sel = module->addWire(NEW_ID);
	Ff_তথ্য ff_sel(module, initvals, NEW_ID);
	Ff_তথ্য ff_তথ্য(module, initvals, NEW_ID);
	ff_sel.width = 1;
	ff_sel.has_clk = সত্য;
	ff_sel.sig_clk = port.clk;
	ff_sel.pol_clk = port.clk_polarity;
	ff_sel.sig_d = port.en;
	ff_sel.sig_q = sel;
	ff_তথ্য.width = Get_আকার(port.তথ্য);
	ff_তথ্য.has_clk = সত্য;
	ff_তথ্য.sig_clk = port.clk;
	ff_তথ্য.pol_clk = port.clk_polarity;
	ff_তথ্য.sig_d = port.তথ্য;
	ff_তথ্য.sig_q = পূর্ব_তথ্য;
	যদি (!port.init_মান.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_তথ্য.val_init = port.init_মান;
		port.init_মান = Const(State::Sx, Get_আকার(port.তথ্য));
	} অন্যথা {
		ff_sel.val_init = State::Sx;
		ff_তথ্য.val_init = Const(State::Sx, Get_আকার(port.তথ্য));
	}
	যদি (port.arst != State::S0) {
		ff_sel.has_arst = সত্য;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = port.arst;
		ff_sel.pol_arst = সত্য;
		ff_তথ্য.has_arst = সত্য;
		ff_তথ্য.val_arst = port.arst_মান;
		ff_তথ্য.sig_arst = port.arst;
		ff_তথ্য.pol_arst = সত্য;
		port.arst = State::S0;
	}
	যদি (port.srst != State::S0) {
		log_assert(!port.ce_over_srst);
		ff_sel.has_srst = সত্য;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = port.srst;
		ff_sel.pol_srst = সত্য;
		ff_sel.ce_over_srst = মিথ্যা;
		ff_তথ্য.has_srst = সত্য;
		ff_তথ্য.val_srst = port.srst_মান;
		ff_তথ্য.sig_srst = port.srst;
		ff_তথ্য.pol_srst = সত্য;
		ff_তথ্য.ce_over_srst = মিথ্যা;
		port.srst = State::S0;
	}
	ff_sel.emit();
	ff_তথ্য.emit();
	module->addMux(NEW_ID, পূর্ব_তথ্য, new_তথ্য, sel, port.তথ্য);
	port.তথ্য = new_তথ্য;
	port.en = State::S1;
}

শূন্য Mem::emulate_reset(পূর্ণসংখ্যা idx, যৌক্তিক emu_init, যৌক্তিক emu_arst, যৌক্তিক emu_srst, FfInitVals *initvals) {
	auto &port = rd_ports[idx];
	যদি (emu_init && !port.init_মান.is_fully_undef()) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_তথ্য ff_sel(module, initvals, NEW_ID);
		Wire *new_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
		ff_sel.width = 1;
		ff_sel.has_clk = সত্য;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		যদি (port.en != State::S1) {
			ff_sel.has_ce = সত্য;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = সত্য;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		যদি (port.arst != State::S0) {
			ff_sel.has_arst = সত্য;
			ff_sel.sig_arst = port.arst;
			ff_sel.pol_arst = সত্য;
			যদি (emu_arst && port.arst_মান == port.init_মান) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				port.arst = State::S0;
			} অন্যথা {
				ff_sel.val_arst = State::S1;
			}
		}
		যদি (port.srst != State::S0) {
			ff_sel.has_srst = সত্য;
			ff_sel.sig_srst = port.srst;
			ff_sel.pol_srst = সত্য;
			যদি (emu_srst && port.srst_মান == port.init_মান) {
				ff_sel.val_srst = State::S0;
				port.srst = State::S0;
			} অন্যথা {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.init_মান, new_তথ্য, sel, port.তথ্য);
		port.তথ্য = new_তথ্য;
		port.init_মান = Const(State::Sx, Get_আকার(port.তথ্য));
	}
	যদি (emu_arst && port.arst != State::S0) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_তথ্য ff_sel(module, initvals, NEW_ID);
		Wire *new_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
		ff_sel.width = 1;
		ff_sel.has_clk = সত্য;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		যদি (port.init_মান.is_fully_undef())
			ff_sel.val_init = State::Sx;
		অন্যথা
			ff_sel.val_init = State::S1;
		যদি (port.en != State::S1) {
			ff_sel.has_ce = সত্য;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = সত্য;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		ff_sel.has_arst = সত্য;
		ff_sel.sig_arst = port.arst;
		ff_sel.pol_arst = সত্য;
		ff_sel.val_arst = State::S0;
		যদি (port.srst != State::S0) {
			ff_sel.has_srst = সত্য;
			ff_sel.sig_srst = port.srst;
			ff_sel.pol_srst = সত্য;
			যদি (emu_srst && port.srst_মান == port.arst_মান) {
				ff_sel.val_srst = State::S0;
				port.srst = State::S0;
			} অন্যথা {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.arst_মান, new_তথ্য, sel, port.তথ্য);
		port.তথ্য = new_তথ্য;
		port.arst = State::S0;
	}
	যদি (emu_srst && port.srst != State::S0) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_তথ্য ff_sel(module, initvals, NEW_ID);
		Wire *new_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
		ff_sel.width = 1;
		ff_sel.has_clk = সত্য;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		যদি (port.init_মান.is_fully_undef())
			ff_sel.val_init = State::Sx;
		অন্যথা
			ff_sel.val_init = State::S1;
		যদি (port.en != State::S1) {
			ff_sel.has_ce = সত্য;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = সত্য;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		ff_sel.has_srst = সত্য;
		ff_sel.sig_srst = port.srst;
		ff_sel.pol_srst = সত্য;
		ff_sel.val_srst = State::S0;
		যদি (port.arst != State::S0) {
			ff_sel.has_arst = সত্য;
			ff_sel.sig_arst = port.arst;
			ff_sel.pol_arst = সত্য;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.srst_মান, new_তথ্য, sel, port.তথ্য);
		port.তথ্য = new_তথ্য;
		port.srst = State::S0;
	}
}

শূন্য Mem::emulate_rd_ce_over_srst(পূর্ণসংখ্যা idx) {
	auto &port = rd_ports[idx];
	log_assert(port.clk_enable);
	যদি (port.en == State::S1 || port.srst == State::S0 || !port.ce_over_srst) {
		port.ce_over_srst = মিথ্যা;
		ফেরত;
	}
	port.ce_over_srst = মিথ্যা;
	port.srst = module->And(NEW_ID, port.en, port.srst);
}

শূন্য Mem::emulate_rd_srst_over_ce(পূর্ণসংখ্যা idx) {
	auto &port = rd_ports[idx];
	log_assert(port.clk_enable);
	যদি (port.en == State::S1 || port.srst == State::S0 || port.ce_over_srst) {
		port.ce_over_srst = সত্য;
		ফেরত;
	}
	port.ce_over_srst = সত্য;
	port.en = module->Or(NEW_ID, port.en, port.srst);
}

যৌক্তিক Mem::emulate_read_first_ok() {
	যদি (wr_ports.empty())
		ফেরত মিথ্যা;
	SigSpec clk = wr_ports[0].clk;
	যৌক্তিক clk_polarity = wr_ports[0].clk_polarity;
	জন্য (auto &port: wr_ports) {
		যদি (!port.clk_enable)
			ফেরত মিথ্যা;
		যদি (port.clk != clk)
			ফেরত মিথ্যা;
		যদি (port.clk_polarity != clk_polarity)
			ফেরত মিথ্যা;
	}
	যৌক্তিক found_read_first = মিথ্যা;
	জন্য (auto &port: rd_ports) {
		যদি (!port.clk_enable)
			ফেরত মিথ্যা;
		যদি (port.clk != clk)
			ফেরত মিথ্যা;
		যদি (port.clk_polarity != clk_polarity)
			ফেরত মিথ্যা;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(wr_ports); j++)
			যদি (!port.transparency_mask[j] && !port.collision_x_mask[j])
				found_read_first = সত্য;
	}
	ফেরত found_read_first;
}

শূন্য Mem::emulate_read_first(FfInitVals *initvals) {
	log_assert(emulate_read_first_ok());
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++)
		জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(wr_ports); j++)
			যদি (rd_ports[i].transparency_mask[j])
				emulate_transparency(j, i, initvals);
	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(rd_ports); i++)
		জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_mask[j]);
			rd_ports[i].collision_x_mask[j] = মিথ্যা;
			rd_ports[i].transparency_mask[j] = সত্য;
		}
	জন্য (auto &port: wr_ports) {
		Wire *new_তথ্য = module->addWire(NEW_ID, Get_আকার(port.তথ্য));
		Wire *new_addr = module->addWire(NEW_ID, Get_আকার(port.addr));
		auto compressed = port.compress_en();
		Wire *new_en = module->addWire(NEW_ID, Get_আকার(compressed.first));
		Ff_তথ্য ff_তথ্য(module, initvals, NEW_ID);
		Ff_তথ্য ff_addr(module, initvals, NEW_ID);
		Ff_তথ্য ff_en(module, initvals, NEW_ID);
		ff_তথ্য.width = Get_আকার(port.তথ্য);
		ff_তথ্য.has_clk = সত্য;
		ff_তথ্য.sig_clk = port.clk;
		ff_তথ্য.pol_clk = port.clk_polarity;
		ff_তথ্য.sig_d = port.তথ্য;
		ff_তথ্য.sig_q = new_তথ্য;;
		ff_তথ্য.val_init = Const(State::Sx, ff_তথ্য.width);
		ff_তথ্য.emit();
		ff_addr.width = Get_আকার(port.addr);
		ff_addr.has_clk = সত্য;
		ff_addr.sig_clk = port.clk;
		ff_addr.pol_clk = port.clk_polarity;
		ff_addr.sig_d = port.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.width);
		ff_addr.emit();
		ff_en.width = Get_আকার(compressed.first);
		ff_en.has_clk = সত্য;
		ff_en.sig_clk = port.clk;
		ff_en.pol_clk = port.clk_polarity;
		ff_en.sig_d = compressed.first;
		ff_en.sig_q = new_en;;
		যদি (inits.empty())
			ff_en.val_init = Const(State::Sx, ff_en.width);
		অন্যথা
			ff_en.val_init = Const(State::S0, ff_en.width);
		ff_en.emit();
		port.তথ্য = new_তথ্য;
		port.addr = new_addr;
		port.en = port.decompress_en(compressed.second, new_en);
	}
}

std::pair<SigSpec, std::vector<পূর্ণসংখ্যা>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::vector<পূর্ণসংখ্যা> swizzle;
	SigBit পূর্ব_bit = en[0];
	পূর্ণসংখ্যা idx = 0;
	জন্য (auto &bit: en) {
		যদি (bit != পূর্ব_bit) {
			sig.append(bit);
			পূর্ব_bit = bit;
			idx++;
		}
		swizzle.ঠেলো_back(idx);
	}
	log_assert(idx + 1 == Get_আকার(sig));
	ফেরত {sig, swizzle};
}

SigSpec MemWr::decompress_en(ধ্রুবক std::vector<পূর্ণসংখ্যা> &swizzle, SigSpec sig) {
	SigSpec res;
	জন্য (পূর্ণসংখ্যা i: swizzle)
		res.append(sig[i]);
	ফেরত res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->আকার_2), mem->width)
{
	জন্য(ধ্রুবক auto &init : mem->inits) {
		যদি(init.en.is_fully_zero()) চালু;
		log_assert(init.en.আকার_2() == তথ্য_width_2);
		যদি(init.en.is_fully_ones())
			ঢোকাও_concatenated(init.addr.as_int(), init.তথ্য);
		অন্যথা {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t শব্দগুলি_2 = init.তথ্য.আকার_2() / তথ্য_width_2;
			RTLIL::Const তথ্য = init.তথ্য;
			log_assert(তথ্য.আকার_2() % তথ্য_width_2 == 0);
			জন্য(addr_t i = 0; i < শব্দগুলি_2; i++) {
				RTLIL::Const previous = (*এই)[addr + i];
				জন্য(পূর্ণসংখ্যা j = 0; j < তথ্য_width_2; j++)
					যদি(init.en[j] != State::S1)
						তথ্য.set(তথ্য_width_2 * i + j, previous[j]);
			}
			ঢোকাও_concatenated(init.addr.as_int(), তথ্য);
		}
	}
}

MemContents::iterator & MemContents::iterator::operator++() {
	auto it = _memory->মানসমূহ_2.upper_bound(_addr);
	যদি(it == _memory->মানসমূহ_2.end()) {
		_memory = nullptr;
		_addr = ~(addr_t) 0;
	} অন্যথা
		_addr = it->first;
	ফেরত *এই;
}

শূন্য MemContents::check() {
	log_assert(_addr_width > 0 && _addr_width < (পূর্ণসংখ্যা)আকার(addr_t) * 8);
	log_assert(তথ্য_width_2 > 0);
	log_assert(default_মান.আকার_2() == তথ্য_width_2);
	যদি(মানসমূহ_2.empty()) ফেরত;
	auto it = মানসমূহ_2.begin();
	জন্য(;;) {
		log_assert(!it->second.empty());
		log_assert(it->second.আকার_2() % তথ্য_width_2 == 0);
		auto end1 = _range_end(it);
		log_assert(_range_begin(it) < (addr_t)(1<<_addr_width));
		log_assert(end1 <= (addr_t)(1<<_addr_width));
		যদি(++it == মানসমূহ_2.end())
			বিরতি;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

যৌক্তিক MemContents::_range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ধ্রুবক {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	ফেরত it != মানসমূহ_2.end() && addr - _range_begin(it) < range_আকার(it);
}


যৌক্তিক MemContents::_range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) ধ্রুবক {
	// note that we assume begin_addr <= end_addr
	ফেরত it != মানসমূহ_2.end() && _range_begin(it) <= begin_addr && end_addr - _range_begin(it) <= range_আকার(it);
}

যৌক্তিক MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) ধ্রুবক {
	যদি(it == মানসমূহ_2.end() || begin_addr >= end_addr)
		ফেরত মিথ্যা;
	auto top1 = _range_end(it) - 1;
	auto top2 = end_addr - 1;
	ফেরত !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ধ্রুবক {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<_addr_width));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	auto it = const_cast<std::map<addr_t, RTLIL::Const> &>(মানসমূহ_2).upper_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	যদি(it == মানসমূহ_2.begin())
		ফেরত it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	auto it_পূর্ব = std::পরবর্তী(it, -1);
	যদি(_range_contains(it_পূর্ব, addr))
		ফেরত it_পূর্ব;
	অন্যথা
		ফেরত it;
}

RTLIL::Const MemContents::operator[](addr_t addr) ধ্রুবক {
	auto it = _range_at(addr);
	যদি(_range_contains(it, addr))
		ফেরত it->second.extract(_range_offset(it, addr), তথ্য_width_2);
	অন্যথা
		ফেরত default_মান;
}

MemContents::addr_t MemContents::গুনতি_range(addr_t begin_addr, addr_t end_addr) ধ্রুবক {
	addr_t গুনতি = 0;
	জন্য(auto it = _range_at(begin_addr); _range_overlaps(it, begin_addr, end_addr); it++) {
		auto first = std::max(_range_begin(it), begin_addr);
		auto last = std::min(_range_end(it), end_addr);
		গুনতি += last - first;
	}
	ফেরত গুনতি;
}

শূন্য MemContents::clear_range(addr_t begin_addr, addr_t end_addr) {
	যদি(begin_addr >= end_addr) ফেরত;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	auto begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	auto end_it = মানসমূহ_2.upper_bound(end_addr - 1);
	যদি(begin_it == end_it)
		ফেরত; // nothing to do
	// the last iterator affected is one before the first one not affected
	auto last_it = std::পরবর্তী(end_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	যদি(_range_contains(last_it, end_addr - 1)) {
		auto new_begin = end_addr;
		auto end = _range_end(last_it);
		// if there is data past the end address, preserve it by creating a new range
		যদি(new_begin != end)
			end_it = মানসমূহ_2.emplace_hint(last_it, new_begin, last_it->second.extract(_range_offset(last_it, new_begin), (_range_end(last_it) - new_begin) * তথ্য_width_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	যদি(_range_contains(begin_it, begin_addr)) {
		auto new_end = begin_addr;
		// if there is data before the start address, truncate but don't delete
		যদি(new_end != begin_it->first) {
			begin_it->second.extu(_range_offset(begin_it, new_end));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	মানসমূহ_2.erase(begin_it, end_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t end_addr) {
	যদি(begin_addr >= end_addr)
		ফেরত মানসমূহ_2.end(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	auto lower_it = begin_addr == 0 ? মানসমূহ_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	যদি (_range_contains(lower_it, begin_addr, end_addr))
		ফেরত lower_it;
	// find the first range containing any addr >= end_addr
	auto upper_it = _range_at(end_addr);
	// check if either of the two ranges we just found touch our range
	যৌক্তিক lower_touch = begin_addr > 0 && _range_contains(lower_it, begin_addr - 1);
	যৌক্তিক upper_touch = _range_contains(upper_it, end_addr);
	যদি (lower_touch && upper_touch) {
		log_assert (lower_it != upper_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		auto upper_end = _range_end(upper_it);
		// make range bigger (maybe reserve here instead of resize?)
		lower_it->second.resize(_range_offset(lower_it, upper_end), State::Sx);
		// copy only the data beyond our range
		std::copy(range_তথ্য(upper_it, end_addr), range_তথ্য(upper_it, upper_end), range_তথ্য(lower_it, end_addr));
		// keep lower_it, but delete upper_it
		মানসমূহ_2.erase(std::পরবর্তী(lower_it), std::পরবর্তী(upper_it));
		ফেরত lower_it;
	} অন্যথা যদি (lower_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		lower_it->second.resize(_range_offset(lower_it, end_addr), State::Sx);
		// keep lower_it and upper_it
		মানসমূহ_2.erase(std::পরবর্তী(lower_it), upper_it);
		ফেরত lower_it;
	} অন্যথা যদি (upper_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const তথ্য(State::Sx, (_range_begin(upper_it) - begin_addr) * তথ্য_width_2);
		তথ্য.append(std::move(upper_it->second));
		// delete lower_it and upper_it, then reinsert
		মানসমূহ_2.erase(lower_it, std::পরবর্তী(upper_it));
		ফেরত মানসমূহ_2.emplace(begin_addr, std::move(তথ্য)).first;
	} অন্যথা {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		মানসমূহ_2.erase(lower_it, upper_it);
		ফেরত মানসমূহ_2.emplace(begin_addr, RTLIL::Const(State::Sx, (end_addr - begin_addr) * তথ্য_width_2)).first;
	}
}

শূন্য MemContents::ঢোকাও_concatenated(addr_t addr, RTLIL::Const ধ্রুবক &মানসমূহ) {
	addr_t শব্দগুলি_2 = (মানসমূহ.আকার_2() + তথ্য_width_2 - 1) / তথ্য_width_2;
	log_assert(addr < (addr_t)(1<<_addr_width));
	log_assert(শব্দগুলি_2 <= (addr_t)(1<<_addr_width) - addr);
	auto it = _reserve_range(addr, addr + শব্দগুলি_2);
	auto to_begin = range_তথ্য(it, addr);
	std::copy(মানসমূহ.begin(), মানসমূহ.end(), to_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(to_begin + মানসমূহ.আকার_2(), to_begin + শব্দগুলি_2 * তথ্য_width_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ধ্রুবক &word) {
	auto from_end = word.আকার_2() <= তথ্য_width_2 ? word.end() : word.begin() + তথ্য_width_2;
	auto to_end = std::copy(word.begin(), from_end, it);
	auto it_পরবর্তী = std::পরবর্তী(it, তথ্য_width_2);
	std::fill(to_end, it_পরবর্তী, State::S0);
	ফেরত it_পরবর্তী;
}
