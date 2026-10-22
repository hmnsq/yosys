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

#جے_بنا_تعریف MEM_H
#تعریف_کرو MEM_H

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
	std::سمتیہ<bool> transparency_نقاب;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::سمتیہ<bool> collision_x_نقاب;
	SigSpec clk, en, arst, srst, addr, انکڑے;

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
	std::سمتیہ<bool> ترجیح_نقاب;
	SigSpec clk, en, addr, انکڑے;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(صحيح sub) {
		SigSpec res = addr;
		لكل (صحيح i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		أرجع res;
	}

	std::pair<SigSpec, std::سمتیہ<صحيح>> compress_en();
	SigSpec decompress_en(ثابت std::سمتیہ<صحيح> &swizzle, SigSpec sig);
};

بنية MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const انکڑے;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

بنية Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::حافظہ *mem;
	Cell *cell;
	صحيح چوڑائی, شروع_وتھ, ناپ;
	std::سمتیہ<MemInit> inits;
	std::سمتیہ<MemRd> rd_ports;
	std::سمتیہ<MemWr> wr_ports;

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
	Const get_init_انکڑے() ثابت;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ساكن std::سمتیہ<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ساكن std::سمتیہ<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *کڈھو_rdff(صحيح idx, FfInitVals *initvals);

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
	فراغ emulate_ترجیح(صحيح idx1, صحيح idx2, FfInitVals *initvals);

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
	فراغ prepare_wr_ملاؤ(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	فراغ prepare_rd_ملاؤ(صحيح idx1, صحيح idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	فراغ widen_prep(صحيح wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	فراغ widen_wr_پورٹ(صحيح idx, صحيح wide_log2);

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
	bool emulate_read_پہلا_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	فراغ emulate_read_پہلا(FfInitVals *initvals);

	Mem(Module *module, IdString memid, صحيح چوڑائی, صحيح شروع_وتھ, صحيح ناپ) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), چوڑائی(چوڑائی), شروع_وتھ(شروع_وتھ), ناپ(ناپ) {}
	Mem(Module *module, TwineSpec &&memid, صحيح چوڑائی, صحيح شروع_وتھ, صحيح ناپ)
			: Mem(module, module->twines().جوڑو(std::move(memid)), چوڑائی, شروع_وتھ, ناپ) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
عام:
	class range; class iterator;
	using addr_t = uint32_t;
خاص:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	صحيح addr_چوڑائی_2;
	صحيح انکڑے_چوڑائی_2;
	RTLIL::Const default_قدر;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> اقدار_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ثابت;
	addr_t range_ناپ(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->دوجا.ناپ() / انکڑے_چوڑائی_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع it->پہلا; }
	addr_t range_سرا(std::map<addr_t, RTLIL::Const>::iterator it) ثابت { أرجع _range_begin(it) + range_ناپ(it); }
	// check if the iterator points to a range containing addr
	bool range_شامل_اے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_شامل_اے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t سرا_addr) ثابت;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t سرا_addr) ثابت;
	// return the offset the addr would have in the range at `it`
	ناپ_t range_وتھ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت { أرجع (addr - it->پہلا) * انکڑے_چوڑائی_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_انکڑے(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		أرجع RTLIL::Const::iterator(it->دوجا, range_وتھ(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t سرا_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &انکڑے);
عام:
	class range {
		صحيح انکڑے_چوڑائی_2;
		addr_t ادھار_2;
		RTLIL::Const ثابت &اقدار_2;
		یار class iterator;
		range(صحيح انکڑے_چوڑائی, addr_t ادھار, RTLIL::Const ثابت &اقدار)
		: انکڑے_چوڑائی_2(انکڑے_چوڑائی), ادھار_2(ادھار), اقدار_2(اقدار) {}
	عام:
		addr_t ادھار() ثابت { أرجع ادھار_2; }
		addr_t ناپ() ثابت { أرجع ((addr_t) اقدار_2.ناپ()) / انکڑے_چوڑائی_2; }
		addr_t حد_بندی() ثابت { أرجع ادھار_2 + ناپ(); }
		RTLIL::Const ثابت &concatenated() ثابت { أرجع اقدار_2; }
		RTLIL::Const عامل[](addr_t addr) ثابت {
			log_assert(addr - ادھار_2 < ناپ());
			أرجع اقدار_2.کڈھو((addr - ادھار_2) * انکڑے_چوڑائی_2, انکڑے_چوڑائی_2);
		}
		RTLIL::Const at_وتھ(addr_t وتھ) ثابت { أرجع (*this)[ادھار_2 + وتھ]; }
	};
	class iterator {
		MemContents ثابت *حافظہ_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		یار class MemContents;
		iterator(MemContents ثابت *حافظہ_3, addr_t addr) : حافظہ_2(حافظہ_3), _addr(addr) {}
	عام:
		using iterator_category = std::داخلہ_iterator_ٹیگ;
		using قدر_type = range;
		using اشارہ_گر = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference عامل *() ثابت { أرجع range(حافظہ_2->انکڑے_چوڑائی_2, _addr, حافظہ_2->اقدار_2.at(_addr)); }
		اشارہ_گر عامل->() ثابت { أرجع arrow_proxy<range>(**this); }
		bool عامل !=(iterator ثابت &other) ثابت { أرجع حافظہ_2 != other.حافظہ_2 || _addr != other._addr; }
		bool عامل ==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
		iterator &عامل++();
	};
	MemContents(صحيح addr_چوڑائی, صحيح انکڑے_چوڑائی, RTLIL::Const default_قدر_2)
		: addr_چوڑائی_2(addr_چوڑائی), انکڑے_چوڑائی_2(انکڑے_چوڑائی)
		, default_قدر((default_قدر_2.extu(انکڑے_چوڑائی), std::move(default_قدر_2)))
	{ log_assert(addr_چوڑائی_2 > 0 && addr_چوڑائی_2 < (صحيح)حجم(addr_t) * 8); log_assert(انکڑے_چوڑائی_2 > 0); }
	MemContents(صحيح addr_چوڑائی, صحيح انکڑے_چوڑائی) : MemContents(addr_چوڑائی, انکڑے_چوڑائی, RTLIL::Const(State::Sx, انکڑے_چوڑائی)) {}
	explicit MemContents(Mem *mem);
	صحيح addr_چوڑائی() ثابت { أرجع addr_چوڑائی_2; }
	صحيح انکڑے_چوڑائی() ثابت { أرجع انکڑے_چوڑائی_2; }
	RTLIL::Const ثابت &default_قدر_2() ثابت { أرجع default_قدر; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const عامل [](addr_t addr) ثابت;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t گنتی_range(addr_t begin_addr, addr_t سرا_addr) ثابت;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	فراغ reserve_range(addr_t begin_addr, addr_t سرا_addr) { _reserve_range(begin_addr, سرا_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	فراغ داخل_کرو_concatenated(addr_t addr, RTLIL::Const ثابت &اقدار);
	// insert multiple words at the given address, overriding any previous assignment.
	سانچا<typename Iterator> فراغ داخل_کرو_range(addr_t addr, Iterator begin, Iterator سرا) {
		خودکار الفاظ_2 = سرا - begin;
		log_assert(addr < (addr_t)(1<<addr_چوڑائی_2)); log_assert(الفاظ_2 <= (addr_t)(1<<addr_چوڑائی_2) - addr);
		خودکار range = _reserve_range(addr, addr + الفاظ_2);
		خودکار it = range_انکڑے(range, addr);
		لكل(; begin != سرا; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	فراغ clear_range(addr_t begin_addr, addr_t سرا_addr);
	// check invariants, abort if invariants failed
	فراغ check();
	iterator سرا() ثابت { أرجع iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ثابت { أرجع اقدار_2.خالی_تھاواں() ? سرا() : iterator(this, اقدار_2.begin()->پہلا); }
	bool خالی_تھاواں() ثابت { أرجع اقدار_2.خالی_تھاواں(); }
};

YOSYS_NAMESPACE_سرا

#جے_انت
