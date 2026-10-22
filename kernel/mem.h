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

#اگر_غیر_تعریف_شدہ MEM_H
#تعریف_کرو MEM_H

#شامل "kernel/yosys.h"
#شامل "kernel/ffinit.h"
#شامل "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

ساخت MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	صحیح wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_قدر, srst_قدر, init_قدر;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::سمتیہ<bool> transparency_نقاب;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::سمتیہ<bool> collision_x_نقاب;
	SigSpec clk, en, arst, srst, addr, ڈیٹا;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحیح sub) {
		SigSpec res = addr;
		برائے (صحیح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		واپس res;
	}
};

ساخت MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	صحیح wide_log2;
	bool clk_enable, clk_polarity;
	std::سمتیہ<bool> ترجیح_نقاب;
	SigSpec clk, en, addr, ڈیٹا;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحیح sub) {
		SigSpec res = addr;
		برائے (صحیح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		واپس res;
	}

	std::pair<SigSpec, std::سمتیہ<صحیح>> compress_en();
	SigSpec decompress_en(ثابت std::سمتیہ<صحیح> &swizzle, SigSpec sig);
};

ساخت MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const ڈیٹا;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

ساخت Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::حافظہ *mem;
	Cell *cell;
	صحیح چوڑائی, آغاز_انحراف, سائز;
	std::سمتیہ<MemInit> inits;
	std::سمتیہ<MemRd> rd_ports;
	std::سمتیہ<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	خالی remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	خالی emit();

	// Marks all inits as removed.
	خالی clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	خالی coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	خالی check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_ڈیٹا() ثابت;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ساکن std::سمتیہ<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ساکن std::سمتیہ<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *نکالو_rdff(صحیح idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	خالی narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	خالی emulate_ترجیح(صحیح idx1, صحیح idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	خالی emulate_transparency(صحیح widx, صحیح ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	خالی prepare_wr_ملاؤ(صحیح idx1, صحیح idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	خالی prepare_rd_ملاؤ(صحیح idx1, صحیح idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	خالی widen_prep(صحیح wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	خالی widen_wr_پورٹ(صحیح idx, صحیح wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	خالی emulate_rden(صحیح idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	خالی emulate_reset(صحیح idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	خالی emulate_rd_ce_over_srst(صحیح idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	خالی emulate_rd_srst_over_ce(صحیح idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_پہلا_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	خالی emulate_read_پہلا(FfInitVals *initvals);

	Mem(Module *module, IdString memid, صحیح چوڑائی, صحیح آغاز_انحراف, صحیح سائز) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), چوڑائی(چوڑائی), آغاز_انحراف(آغاز_انحراف), سائز(سائز) {}
	Mem(Module *module, TwineSpec &&memid, صحیح چوڑائی, صحیح آغاز_انحراف, صحیح سائز)
			: Mem(module, module->twines().جوڑو(std::move(memid)), چوڑائی, آغاز_انحراف, سائز) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
عوامی:
	class range; class iterator;
	using addr_t = uint32_t;
نجی:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	صحیح addr_چوڑائی_2;
	صحیح ڈیٹا_چوڑائی_2;
	RTLIL::Const default_قدر;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> اقدار_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ثابت;
	addr_t range_سائز(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { واپس it->دوسرا.سائز() / ڈیٹا_چوڑائی_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { واپس it->پہلا; }
	addr_t range_انتہا(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { واپس _range_begin(it) + range_سائز(it); }
	// check if the iterator points to a range containing addr
	bool range_شامل_ہے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_شامل_ہے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتہا_addr) ثابت;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t انتہا_addr) ثابت;
	// return the offset the addr would have in the range at `it`
	سائز_t range_انحراف(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت { واپس (addr - it->پہلا) * ڈیٹا_چوڑائی_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_ڈیٹا(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		واپس RTLIL::Const::iterator(it->دوسرا, range_انحراف(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t انتہا_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &ڈیٹا);
عوامی:
	class range {
		صحیح ڈیٹا_چوڑائی_2;
		addr_t بنیاد_2;
		RTLIL::Const ثابت &اقدار_2;
		دوست class iterator;
		range(صحیح ڈیٹا_چوڑائی, addr_t بنیاد, RTLIL::Const ثابت &اقدار)
		: ڈیٹا_چوڑائی_2(ڈیٹا_چوڑائی), بنیاد_2(بنیاد), اقدار_2(اقدار) {}
	عوامی:
		addr_t بنیاد() ثابت { واپس بنیاد_2; }
		addr_t سائز() ثابت { واپس ((addr_t) اقدار_2.سائز()) / ڈیٹا_چوڑائی_2; }
		addr_t حد_بندی() ثابت { واپس بنیاد_2 + سائز(); }
		RTLIL::Const ثابت &concatenated() ثابت { واپس اقدار_2; }
		RTLIL::Const عامل[](addr_t addr) ثابت {
			log_assert(addr - بنیاد_2 < سائز());
			واپس اقدار_2.نکالو_2((addr - بنیاد_2) * ڈیٹا_چوڑائی_2, ڈیٹا_چوڑائی_2);
		}
		RTLIL::Const at_انحراف(addr_t انحراف) ثابت { واپس (*this)[بنیاد_2 + انحراف]; }
	};
	class iterator {
		MemContents ثابت *حافظہ_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		دوست class MemContents;
		iterator(MemContents ثابت *حافظہ_3, addr_t addr) : حافظہ_2(حافظہ_3), _addr(addr) {}
	عوامی:
		using iterator_category = std::داخلہ_iterator_لیبل;
		using قدر_type = range;
		using اشارہ_گر = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference عامل *() ثابت { واپس range(حافظہ_2->ڈیٹا_چوڑائی_2, _addr, حافظہ_2->اقدار_2.at(_addr)); }
		اشارہ_گر عامل->() ثابت { واپس arrow_proxy<range>(**this); }
		bool عامل !=(iterator ثابت &other) ثابت { واپس حافظہ_2 != other.حافظہ_2 || _addr != other._addr; }
		bool عامل ==(iterator ثابت &other) ثابت { واپس !(*this != other); }
		iterator &عامل++();
	};
	MemContents(صحیح addr_چوڑائی, صحیح ڈیٹا_چوڑائی, RTLIL::Const default_قدر_2)
		: addr_چوڑائی_2(addr_چوڑائی), ڈیٹا_چوڑائی_2(ڈیٹا_چوڑائی)
		, default_قدر((default_قدر_2.extu(ڈیٹا_چوڑائی), std::move(default_قدر_2)))
	{ log_assert(addr_چوڑائی_2 > 0 && addr_چوڑائی_2 < (صحیح)حجم(addr_t) * 8); log_assert(ڈیٹا_چوڑائی_2 > 0); }
	MemContents(صحیح addr_چوڑائی, صحیح ڈیٹا_چوڑائی) : MemContents(addr_چوڑائی, ڈیٹا_چوڑائی, RTLIL::Const(State::Sx, ڈیٹا_چوڑائی)) {}
	explicit MemContents(Mem *mem);
	صحیح addr_چوڑائی() ثابت { واپس addr_چوڑائی_2; }
	صحیح ڈیٹا_چوڑائی() ثابت { واپس ڈیٹا_چوڑائی_2; }
	RTLIL::Const ثابت &default_قدر_2() ثابت { واپس default_قدر; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const عامل [](addr_t addr) ثابت;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t گنتی_range(addr_t begin_addr, addr_t انتہا_addr) ثابت;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	خالی reserve_range(addr_t begin_addr, addr_t انتہا_addr) { _reserve_range(begin_addr, انتہا_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	خالی داخل_کرو_concatenated(addr_t addr, RTLIL::Const ثابت &اقدار);
	// insert multiple words at the given address, overriding any previous assignment.
	سانچہ<typename Iterator> خالی داخل_کرو_range(addr_t addr, Iterator begin, Iterator انتہا) {
		خودکار الفاظ_2 = انتہا - begin;
		log_assert(addr < (addr_t)(1<<addr_چوڑائی_2)); log_assert(الفاظ_2 <= (addr_t)(1<<addr_چوڑائی_2) - addr);
		خودکار range = _reserve_range(addr, addr + الفاظ_2);
		خودکار it = range_ڈیٹا(range, addr);
		برائے(; begin != انتہا; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	خالی clear_range(addr_t begin_addr, addr_t انتہا_addr);
	// check invariants, abort if invariants failed
	خالی check();
	iterator انتہا() ثابت { واپس iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ثابت { واپس اقدار_2.خالی_خانے() ? انتہا() : iterator(this, اقدار_2.begin()->پہلا); }
	bool خالی_خانے() ثابت { واپس اقدار_2.خالی_خانے(); }
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
