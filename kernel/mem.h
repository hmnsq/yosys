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

#اگر_تعریف_نشده MEM_H
#تعریف_کن MEM_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/ffinit.h"
#ضمن "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

بنية MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	صحيح wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_مقدار, srst_مقدار, init_مقدار;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::وکتور<bool> transparency_ماسک;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::وکتور<bool> collision_x_ماسک;
	SigSpec clk, en, arst, srst, addr, داده;

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
	std::وکتور<bool> اولویت_ماسک;
	SigSpec clk, en, addr, داده;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحيح sub) {
		SigSpec res = addr;
		لكل (صحيح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		أرجع res;
	}

	std::pair<SigSpec, std::وکتور<صحيح>> compress_en();
	SigSpec decompress_en(ثابت std::وکتور<صحيح> &swizzle, SigSpec sig);
};

بنية MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const داده;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

بنية Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::حافظه *mem;
	Cell *cell;
	صحيح عرض, شروع_جابه_جایی, اندازه;
	std::وکتور<MemInit> inits;
	std::وکتور<MemRd> rd_ports;
	std::وکتور<MemWr> wr_ports;

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
	Const get_init_داده() ثابت;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ساكن std::وکتور<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ساكن std::وکتور<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *استخراج_rdff(صحيح idx, FfInitVals *initvals);

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
	فراغ emulate_اولویت(صحيح idx1, صحيح idx2, FfInitVals *initvals);

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
	فراغ prepare_wr_ادغام(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	فراغ prepare_rd_ادغام(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	فراغ widen_prep(صحيح wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	فراغ widen_wr_پورت(صحيح idx, صحيح wide_log2);

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
	bool emulate_read_اولی_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	فراغ emulate_read_اولی(FfInitVals *initvals);

	Mem(Module *module, IdString memid, صحيح عرض, صحيح شروع_جابه_جایی, صحيح اندازه) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), عرض(عرض), شروع_جابه_جایی(شروع_جابه_جایی), اندازه(اندازه) {}
	Mem(Module *module, TwineSpec &&memid, صحيح عرض, صحيح شروع_جابه_جایی, صحيح اندازه)
			: Mem(module, module->twines().جمع_کن(std::move(memid)), عرض, شروع_جابه_جایی, اندازه) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
عام:
	class range; class iterator;
	using addr_t = uint32_t;
خاص:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	صحيح addr_عرض_2;
	صحيح داده_عرض_2;
	RTLIL::Const default_مقدار;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> مقادیر_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ثابت;
	addr_t range_اندازه(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->دومی.اندازه() / داده_عرض_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->اولی; }
	addr_t range_انتها(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع _range_begin(it) + range_اندازه(it); }
	// check if the iterator points to a range containing addr
	bool range_شامل_است(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_شامل_است(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتها_addr) ثابت;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتها_addr) ثابت;
	// return the offset the addr would have in the range at `it`
	اندازه_t range_جابه_جایی(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت { أرجع (addr - it->اولی) * داده_عرض_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_داده(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		أرجع RTLIL::Const::iterator(it->دومی, range_جابه_جایی(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t انتها_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &داده);
عام:
	class range {
		صحيح داده_عرض_2;
		addr_t پایه_2;
		RTLIL::Const ثابت &مقادیر_2;
		دوست class iterator;
		range(صحيح داده_عرض, addr_t پایه, RTLIL::Const ثابت &مقادیر)
		: داده_عرض_2(داده_عرض), پایه_2(پایه), مقادیر_2(مقادیر) {}
	عام:
		addr_t پایه() ثابت { أرجع پایه_2; }
		addr_t اندازه() ثابت { أرجع ((addr_t) مقادیر_2.اندازه()) / داده_عرض_2; }
		addr_t حد() ثابت { أرجع پایه_2 + اندازه(); }
		RTLIL::Const ثابت &concatenated() ثابت { أرجع مقادیر_2; }
		RTLIL::Const عملگر[](addr_t addr) ثابت {
			log_assert(addr - پایه_2 < اندازه());
			أرجع مقادیر_2.استخراج((addr - پایه_2) * داده_عرض_2, داده_عرض_2);
		}
		RTLIL::Const at_جابه_جایی(addr_t جابه_جایی) ثابت { أرجع (*this)[پایه_2 + جابه_جایی]; }
	};
	class iterator {
		MemContents ثابت *حافظه_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		دوست class MemContents;
		iterator(MemContents ثابت *حافظه_3, addr_t addr) : حافظه_2(حافظه_3), _addr(addr) {}
	عام:
		using iterator_category = std::درون_داد_iterator_برچسب;
		using مقدار_type = range;
		using اشاره_گر = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference عملگر *() ثابت { أرجع range(حافظه_2->داده_عرض_2, _addr, حافظه_2->مقادیر_2.at(_addr)); }
		اشاره_گر عملگر->() ثابت { أرجع arrow_proxy<range>(**this); }
		bool عملگر !=(iterator ثابت &other) ثابت { أرجع حافظه_2 != other.حافظه_2 || _addr != other._addr; }
		bool عملگر ==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
		iterator &عملگر++();
	};
	MemContents(صحيح addr_عرض, صحيح داده_عرض, RTLIL::Const default_مقدار_2)
		: addr_عرض_2(addr_عرض), داده_عرض_2(داده_عرض)
		, default_مقدار((default_مقدار_2.extu(داده_عرض), std::move(default_مقدار_2)))
	{ log_assert(addr_عرض_2 > 0 && addr_عرض_2 < (صحيح)حجم(addr_t) * 8); log_assert(داده_عرض_2 > 0); }
	MemContents(صحيح addr_عرض, صحيح داده_عرض) : MemContents(addr_عرض, داده_عرض, RTLIL::Const(State::Sx, داده_عرض)) {}
	explicit MemContents(Mem *mem);
	صحيح addr_عرض() ثابت { أرجع addr_عرض_2; }
	صحيح داده_عرض() ثابت { أرجع داده_عرض_2; }
	RTLIL::Const ثابت &default_مقدار_2() ثابت { أرجع default_مقدار; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const عملگر [](addr_t addr) ثابت;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t شمار_range(addr_t begin_addr, addr_t انتها_addr) ثابت;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	فراغ reserve_range(addr_t begin_addr, addr_t انتها_addr) { _reserve_range(begin_addr, انتها_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	فراغ درج_concatenated(addr_t addr, RTLIL::Const ثابت &مقادیر);
	// insert multiple words at the given address, overriding any previous assignment.
	قالب<typename Iterator> فراغ درج_range(addr_t addr, Iterator begin, Iterator انتها) {
		خودکار واژگان_2 = انتها - begin;
		log_assert(addr < (addr_t)(1<<addr_عرض_2)); log_assert(واژگان_2 <= (addr_t)(1<<addr_عرض_2) - addr);
		خودکار range = _reserve_range(addr, addr + واژگان_2);
		خودکار it = range_داده(range, addr);
		لكل(; begin != انتها; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	فراغ clear_range(addr_t begin_addr, addr_t انتها_addr);
	// check invariants, abort if invariants failed
	فراغ check();
	iterator انتها() ثابت { أرجع iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ثابت { أرجع مقادیر_2.خالی() ? انتها() : iterator(this, مقادیر_2.begin()->اولی); }
	bool خالی() ثابت { أرجع مقادیر_2.خالی(); }
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
