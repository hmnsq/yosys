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

#جيڪڏهن_وصف_نه_ٿيل MEM_H
#وصف_ڪريو MEM_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/ffinit.h"
#ضمن "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

بنية MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	صحيح wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_قدر, srst_قدر, init_قدر;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::ويڪٽر<bool> transparency_نقاب;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::ويڪٽر<bool> collision_x_نقاب;
	SigSpec clk, en, arst, srst, addr, معلومات;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحيح sub) {
		SigSpec res = addr;
		لكل (صحيح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		أرجع res;
	}
};

بنية MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	صحيح wide_log2;
	bool clk_enable, clk_polarity;
	std::ويڪٽر<bool> ترجيح_نقاب;
	SigSpec clk, en, addr, معلومات;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحيح sub) {
		SigSpec res = addr;
		لكل (صحيح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		أرجع res;
	}

	std::pair<SigSpec, std::ويڪٽر<صحيح>> compress_en();
	SigSpec decompress_en(ثابت std::ويڪٽر<صحيح> &swizzle, SigSpec sig);
};

بنية MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const معلومات;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

بنية Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::يادگيري *mem;
	Cell *cell;
	صحيح ويڪر, شروعات_وٿي, سائيز;
	std::ويڪٽر<MemInit> inits;
	std::ويڪٽر<MemRd> rd_ports;
	std::ويڪٽر<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	فراغ remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	فراغ emit();

	// Marks all inits as removed.
	فراغ clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	فراغ coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	فراغ check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_معلومات() ثابت;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ساكن std::ويڪٽر<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ساكن std::ويڪٽر<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *ڪڍو_rdff(صحيح idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	فراغ narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	فراغ emulate_ترجيح(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	فراغ emulate_transparency(صحيح widx, صحيح ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	فراغ prepare_wr_ملايو(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	فراغ prepare_rd_ملايو(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	فراغ widen_prep(صحيح wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	فراغ widen_wr_پورٽ(صحيح idx, صحيح wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	فراغ emulate_rden(صحيح idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	فراغ emulate_reset(صحيح idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	فراغ emulate_rd_ce_over_srst(صحيح idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	فراغ emulate_rd_srst_over_ce(صحيح idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_پهريون_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	فراغ emulate_read_پهريون(FfInitVals *initvals);

	Mem(Module *module, IdString memid, صحيح ويڪر, صحيح شروعات_وٿي, صحيح سائيز) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), ويڪر(ويڪر), شروعات_وٿي(شروعات_وٿي), سائيز(سائيز) {}
	Mem(Module *module, TwineSpec &&memid, صحيح ويڪر, صحيح شروعات_وٿي, صحيح سائيز)
			: Mem(module, module->twines().جوڙيو(std::move(memid)), ويڪر, شروعات_وٿي, سائيز) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
عام:
	class range; class iterator;
	using addr_t = uint32_t;
خاص:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	صحيح addr_ويڪر_2;
	صحيح معلومات_ويڪر_2;
	RTLIL::Const default_قدر;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> قدرون_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ثابت;
	addr_t range_سائيز(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->ٻيو.سائيز() / معلومات_ويڪر_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->پهريون; }
	addr_t range_پڇاڙي(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع _range_begin(it) + range_سائيز(it); }
	// check if the iterator points to a range containing addr
	bool range_شامل_آهي(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_شامل_آهي(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پڇاڙي_addr) ثابت;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پڇاڙي_addr) ثابت;
	// return the offset the addr would have in the range at `it`
	سائيز_t range_وٿي(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت { أرجع (addr - it->پهريون) * معلومات_ويڪر_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_معلومات(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		أرجع RTLIL::Const::iterator(it->ٻيو, range_وٿي(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t پڇاڙي_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &معلومات);
عام:
	class range {
		صحيح معلومات_ويڪر_2;
		addr_t بنياد_2;
		RTLIL::Const ثابت &قدرون_2;
		دوست class iterator;
		range(صحيح معلومات_ويڪر, addr_t بنياد, RTLIL::Const ثابت &قدرون)
		: معلومات_ويڪر_2(معلومات_ويڪر), بنياد_2(بنياد), قدرون_2(قدرون) {}
	عام:
		addr_t بنياد() ثابت { أرجع بنياد_2; }
		addr_t سائيز() ثابت { أرجع ((addr_t) قدرون_2.سائيز()) / معلومات_ويڪر_2; }
		addr_t حد_بندي() ثابت { أرجع بنياد_2 + سائيز(); }
		RTLIL::Const ثابت &concatenated() ثابت { أرجع قدرون_2; }
		RTLIL::Const عامل[](addr_t addr) ثابت {
			log_assert(addr - بنياد_2 < سائيز());
			أرجع قدرون_2.ڪڍو_2((addr - بنياد_2) * معلومات_ويڪر_2, معلومات_ويڪر_2);
		}
		RTLIL::Const at_وٿي(addr_t وٿي) ثابت { أرجع (*this)[بنياد_2 + وٿي]; }
	};
	class iterator {
		MemContents ثابت *يادگيري_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		دوست class MemContents;
		iterator(MemContents ثابت *يادگيري_3, addr_t addr) : يادگيري_2(يادگيري_3), _addr(addr) {}
	عام:
		using iterator_category = std::داخلا_iterator_ٽيگ;
		using قدر_type = range;
		using اشارو = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference عامل *() ثابت { أرجع range(يادگيري_2->معلومات_ويڪر_2, _addr, يادگيري_2->قدرون_2.at(_addr)); }
		اشارو عامل->() ثابت { أرجع arrow_proxy<range>(**this); }
		bool عامل !=(iterator ثابت &other) ثابت { أرجع يادگيري_2 != other.يادگيري_2 || _addr != other._addr; }
		bool عامل ==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
		iterator &عامل++();
	};
	MemContents(صحيح addr_ويڪر, صحيح معلومات_ويڪر, RTLIL::Const default_قدر_2)
		: addr_ويڪر_2(addr_ويڪر), معلومات_ويڪر_2(معلومات_ويڪر)
		, default_قدر((default_قدر_2.extu(معلومات_ويڪر), std::move(default_قدر_2)))
	{ log_assert(addr_ويڪر_2 > 0 && addr_ويڪر_2 < (صحيح)حجم(addr_t) * 8); log_assert(معلومات_ويڪر_2 > 0); }
	MemContents(صحيح addr_ويڪر, صحيح معلومات_ويڪر) : MemContents(addr_ويڪر, معلومات_ويڪر, RTLIL::Const(State::Sx, معلومات_ويڪر)) {}
	explicit MemContents(Mem *mem);
	صحيح addr_ويڪر() ثابت { أرجع addr_ويڪر_2; }
	صحيح معلومات_ويڪر() ثابت { أرجع معلومات_ويڪر_2; }
	RTLIL::Const ثابت &default_قدر_2() ثابت { أرجع default_قدر; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const عامل [](addr_t addr) ثابت;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t ڳڻپ_range(addr_t begin_addr, addr_t پڇاڙي_addr) ثابت;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	فراغ reserve_range(addr_t begin_addr, addr_t پڇاڙي_addr) { _reserve_range(begin_addr, پڇاڙي_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	فراغ وجهو_concatenated(addr_t addr, RTLIL::Const ثابت &قدرون);
	// insert multiple words at the given address, overriding any previous assignment.
	سانچو<typename Iterator> فراغ وجهو_range(addr_t addr, Iterator begin, Iterator پڇاڙي) {
		خودڪار لفظ_3 = پڇاڙي - begin;
		log_assert(addr < (addr_t)(1<<addr_ويڪر_2)); log_assert(لفظ_3 <= (addr_t)(1<<addr_ويڪر_2) - addr);
		خودڪار range = _reserve_range(addr, addr + لفظ_3);
		خودڪار it = range_معلومات(range, addr);
		لكل(; begin != پڇاڙي; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	فراغ clear_range(addr_t begin_addr, addr_t پڇاڙي_addr);
	// check invariants, abort if invariants failed
	فراغ check();
	iterator پڇاڙي() ثابت { أرجع iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ثابت { أرجع قدرون_2.خالي_جايون() ? پڇاڙي() : iterator(this, قدرون_2.begin()->پهريون); }
	bool خالي_جايون() ثابت { أرجع قدرون_2.خالي_جايون(); }
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
