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

#నిర్వచితం_కాకపోతే MEM_H
#నిర్వచించు MEM_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/ffinit.h"
#అన్తర్భావ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	పూర్ణాంకం wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_విలువ, srst_విలువ, init_విలువ;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::సదిశ<bool> transparency_ముసుగు;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::సదిశ<bool> collision_x_ముసుగు;
	SigSpec clk, en, arst, srst, addr, దత్తాంశం;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(పూర్ణాంకం sub) {
		SigSpec res = addr;
		కొరకు (పూర్ణాంకం i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		తిరిగి res;
	}
};

నిర్మాణం MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	పూర్ణాంకం wide_log2;
	bool clk_enable, clk_polarity;
	std::సదిశ<bool> ప్రాధాన్యత_ముసుగు;
	SigSpec clk, en, addr, దత్తాంశం;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(పూర్ణాంకం sub) {
		SigSpec res = addr;
		కొరకు (పూర్ణాంకం i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		తిరిగి res;
	}

	std::pair<SigSpec, std::సదిశ<పూర్ణాంకం>> compress_en();
	SigSpec decompress_en(స్థిరరాశి std::సదిశ<పూర్ణాంకం> &swizzle, SigSpec sig);
};

నిర్మాణం MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const దత్తాంశం;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

నిర్మాణం Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::జ్ఞాపకం *mem;
	Cell *cell;
	పూర్ణాంకం వెడల్పు, ప్రారంభం_స్థానభ్రంశం, కొలత;
	std::సదిశ<MemInit> inits;
	std::సదిశ<MemRd> rd_ports;
	std::సదిశ<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	శూన్య remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	శూన్య emit();

	// Marks all inits as removed.
	శూన్య clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	శూన్య coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	శూన్య check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_దత్తాంశం() స్థిరరాశి;

	// Constructs and returns the helper structures for all memories
	// in a module.
	స్థిర std::సదిశ<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	స్థిర std::సదిశ<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *వెలికితీయి_rdff(పూర్ణాంకం idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	శూన్య narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	శూన్య emulate_ప్రాధాన్యత(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	శూన్య emulate_transparency(పూర్ణాంకం widx, పూర్ణాంకం ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	శూన్య prepare_wr_విలీనం(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	శూన్య prepare_rd_విలీనం(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	శూన్య widen_prep(పూర్ణాంకం wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	శూన్య widen_wr_పోర్టు(పూర్ణాంకం idx, పూర్ణాంకం wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	శూన్య emulate_rden(పూర్ణాంకం idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	శూన్య emulate_reset(పూర్ణాంకం idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	శూన్య emulate_rd_ce_over_srst(పూర్ణాంకం idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	శూన్య emulate_rd_srst_over_ce(పూర్ణాంకం idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_మొదటి_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	శూన్య emulate_read_మొదటి(FfInitVals *initvals);

	Mem(Module *module, IdString memid, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ప్రారంభం_స్థానభ్రంశం, పూర్ణాంకం కొలత) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), వెడల్పు(వెడల్పు), ప్రారంభం_స్థానభ్రంశం(ప్రారంభం_స్థానభ్రంశం), కొలత(కొలత) {}
	Mem(Module *module, TwineSpec &&memid, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ప్రారంభం_స్థానభ్రంశం, పూర్ణాంకం కొలత)
			: Mem(module, module->twines().కూడు(std::move(memid)), వెడల్పు, ప్రారంభం_స్థానభ్రంశం, కొలత) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
బహిరంగ:
	class range; class iterator;
	using addr_t = uint32_t;
అంతరంగ:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	పూర్ణాంకం addr_వెడల్పు_2;
	పూర్ణాంకం దత్తాంశం_వెడల్పు_2;
	RTLIL::Const default_విలువ;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> విలువలు_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) స్థిరరాశి;
	addr_t range_కొలత(std::map<addr_t, RTLIL::Const>::iterator it) స్థిరరాశి { తిరిగి it->రెండవ.కొలత() / దత్తాంశం_వెడల్పు_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) స్థిరరాశి { తిరిగి it->మొదటి; }
	addr_t range_చివర(std::map<addr_t, RTLIL::Const>::iterator it) స్థిరరాశి { తిరిగి _range_begin(it) + range_కొలత(it); }
	// check if the iterator points to a range containing addr
	bool range_కలిగియుంది(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) స్థిరరాశి;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_కలిగియుంది(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి;
	// return the offset the addr would have in the range at `it`
	కొలత_t range_స్థానభ్రంశం(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) స్థిరరాశి { తిరిగి (addr - it->మొదటి) * దత్తాంశం_వెడల్పు_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_దత్తాంశం(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		తిరిగి RTLIL::Const::iterator(it->రెండవ, range_స్థానభ్రంశం(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t చివర_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const స్థిరరాశి &దత్తాంశం);
బహిరంగ:
	class range {
		పూర్ణాంకం దత్తాంశం_వెడల్పు_2;
		addr_t పునాది_2;
		RTLIL::Const స్థిరరాశి &విలువలు_2;
		మిత్ర class iterator;
		range(పూర్ణాంకం దత్తాంశం_వెడల్పు, addr_t పునాది, RTLIL::Const స్థిరరాశి &విలువలు)
		: దత్తాంశం_వెడల్పు_2(దత్తాంశం_వెడల్పు), పునాది_2(పునాది), విలువలు_2(విలువలు) {}
	బహిరంగ:
		addr_t పునాది() స్థిరరాశి { తిరిగి పునాది_2; }
		addr_t కొలత() స్థిరరాశి { తిరిగి ((addr_t) విలువలు_2.కొలత()) / దత్తాంశం_వెడల్పు_2; }
		addr_t హద్దు() స్థిరరాశి { తిరిగి పునాది_2 + కొలత(); }
		RTLIL::Const స్థిరరాశి &concatenated() స్థిరరాశి { తిరిగి విలువలు_2; }
		RTLIL::Const క్రియాకారకం[](addr_t addr) స్థిరరాశి {
			log_assert(addr - పునాది_2 < కొలత());
			తిరిగి విలువలు_2.వెలికితీయి((addr - పునాది_2) * దత్తాంశం_వెడల్పు_2, దత్తాంశం_వెడల్పు_2);
		}
		RTLIL::Const at_స్థానభ్రంశం(addr_t స్థానభ్రంశం) స్థిరరాశి { తిరిగి (*this)[పునాది_2 + స్థానభ్రంశం]; }
	};
	class iterator {
		MemContents స్థిరరాశి *జ్ఞాపకం_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		మిత్ర class MemContents;
		iterator(MemContents స్థిరరాశి *జ్ఞాపకం_3, addr_t addr) : జ్ఞాపకం_2(జ్ఞాపకం_3), _addr(addr) {}
	బహిరంగ:
		using iterator_category = std::ఆదానం_iterator_ట్యాగ్;
		using విలువ_type = range;
		using సూచిక = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference క్రియాకారకం *() స్థిరరాశి { తిరిగి range(జ్ఞాపకం_2->దత్తాంశం_వెడల్పు_2, _addr, జ్ఞాపకం_2->విలువలు_2.at(_addr)); }
		సూచిక క్రియాకారకం->() స్థిరరాశి { తిరిగి arrow_proxy<range>(**this); }
		bool క్రియాకారకం !=(iterator స్థిరరాశి &other) స్థిరరాశి { తిరిగి జ్ఞాపకం_2 != other.జ్ఞాపకం_2 || _addr != other._addr; }
		bool క్రియాకారకం ==(iterator స్థిరరాశి &other) స్థిరరాశి { తిరిగి !(*this != other); }
		iterator &క్రియాకారకం++();
	};
	MemContents(పూర్ణాంకం addr_వెడల్పు, పూర్ణాంకం దత్తాంశం_వెడల్పు, RTLIL::Const default_విలువ_2)
		: addr_వెడల్పు_2(addr_వెడల్పు), దత్తాంశం_వెడల్పు_2(దత్తాంశం_వెడల్పు)
		, default_విలువ((default_విలువ_2.extu(దత్తాంశం_వెడల్పు), std::move(default_విలువ_2)))
	{ log_assert(addr_వెడల్పు_2 > 0 && addr_వెడల్పు_2 < (పూర్ణాంకం)పరిమాణం(addr_t) * 8); log_assert(దత్తాంశం_వెడల్పు_2 > 0); }
	MemContents(పూర్ణాంకం addr_వెడల్పు, పూర్ణాంకం దత్తాంశం_వెడల్పు) : MemContents(addr_వెడల్పు, దత్తాంశం_వెడల్పు, RTLIL::Const(State::Sx, దత్తాంశం_వెడల్పు)) {}
	explicit MemContents(Mem *mem);
	పూర్ణాంకం addr_వెడల్పు() స్థిరరాశి { తిరిగి addr_వెడల్పు_2; }
	పూర్ణాంకం దత్తాంశం_వెడల్పు() స్థిరరాశి { తిరిగి దత్తాంశం_వెడల్పు_2; }
	RTLIL::Const స్థిరరాశి &default_విలువ_2() స్థిరరాశి { తిరిగి default_విలువ; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const క్రియాకారకం [](addr_t addr) స్థిరరాశి;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t లెక్క_range(addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	శూన్య reserve_range(addr_t begin_addr, addr_t చివర_addr) { _reserve_range(begin_addr, చివర_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	శూన్య చొప్పించు_concatenated(addr_t addr, RTLIL::Const స్థిరరాశి &విలువలు);
	// insert multiple words at the given address, overriding any previous assignment.
	మూస<typename Iterator> శూన్య చొప్పించు_range(addr_t addr, Iterator begin, Iterator చివర) {
		స్వయంచాలక పదాలు_2 = చివర - begin;
		log_assert(addr < (addr_t)(1<<addr_వెడల్పు_2)); log_assert(పదాలు_2 <= (addr_t)(1<<addr_వెడల్పు_2) - addr);
		స్వయంచాలక range = _reserve_range(addr, addr + పదాలు_2);
		స్వయంచాలక it = range_దత్తాంశం(range, addr);
		కొరకు(; begin != చివర; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	శూన్య clear_range(addr_t begin_addr, addr_t చివర_addr);
	// check invariants, abort if invariants failed
	శూన్య check();
	iterator చివర() స్థిరరాశి { తిరిగి iterator(nullptr, ~(addr_t) 0); }
	iterator begin() స్థిరరాశి { తిరిగి విలువలు_2.ఖాళీలు() ? చివర() : iterator(this, విలువలు_2.begin()->మొదటి); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి విలువలు_2.ఖాళీలు(); }
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
