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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MEM_H
#ವ್ಯಾಖ್ಯಾನಿಸು MEM_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/ffinit.h"
#ಅನ್ತರ್ಭಾವ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ಪೂರ್ಣಾಂಕ wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_ಮೌಲ್ಯ, srst_ಮೌಲ್ಯ, init_ಮೌಲ್ಯ;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::ಸದಿಶ<bool> transparency_ಮುಖವಾಡ;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::ಸದಿಶ<bool> collision_x_ಮುಖವಾಡ;
	SigSpec clk, en, arst, srst, addr, ದತ್ತಾಂಶ;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(ಪೂರ್ಣಾಂಕ sub) {
		SigSpec res = addr;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ಹಿಂತಿರುಗಿ res;
	}
};

ರಚನೆ MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ಪೂರ್ಣಾಂಕ wide_log2;
	bool clk_enable, clk_polarity;
	std::ಸದಿಶ<bool> ಆದ್ಯತೆ_ಮುಖವಾಡ;
	SigSpec clk, en, addr, ದತ್ತಾಂಶ;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(ಪೂರ್ಣಾಂಕ sub) {
		SigSpec res = addr;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ಹಿಂತಿರುಗಿ res;
	}

	std::pair<SigSpec, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>> compress_en();
	SigSpec decompress_en(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &swizzle, SigSpec sig);
};

ರಚನೆ MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const ದತ್ತಾಂಶ;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

ರಚನೆ Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::ಸ್ಮರಣೆ *mem;
	Cell *cell;
	ಪೂರ್ಣಾಂಕ ಅಗಲ, ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2;
	std::ಸದಿಶ<MemInit> inits;
	std::ಸದಿಶ<MemRd> rd_ports;
	std::ಸದಿಶ<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	ಶೂನ್ಯ remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	ಶೂನ್ಯ emit();

	// Marks all inits as removed.
	ಶೂನ್ಯ clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	ಶೂನ್ಯ coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	ಶೂನ್ಯ check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ಸ್ಥಿರ std::ಸದಿಶ<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ಸ್ಥಿರ std::ಸದಿಶ<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *ಹೊರತೆಗೆ_rdff(ಪೂರ್ಣಾಂಕ idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	ಶೂನ್ಯ narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	ಶೂನ್ಯ emulate_ಆದ್ಯತೆ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	ಶೂನ್ಯ emulate_transparency(ಪೂರ್ಣಾಂಕ widx, ಪೂರ್ಣಾಂಕ ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	ಶೂನ್ಯ prepare_wr_ವಿಲೀನ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	ಶೂನ್ಯ prepare_rd_ವಿಲೀನ(ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	ಶೂನ್ಯ widen_prep(ಪೂರ್ಣಾಂಕ wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	ಶೂನ್ಯ widen_wr_ಪೋರ್ಟ್(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	ಶೂನ್ಯ emulate_rden(ಪೂರ್ಣಾಂಕ idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	ಶೂನ್ಯ emulate_reset(ಪೂರ್ಣಾಂಕ idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	ಶೂನ್ಯ emulate_rd_ce_over_srst(ಪೂರ್ಣಾಂಕ idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	ಶೂನ್ಯ emulate_rd_srst_over_ce(ಪೂರ್ಣಾಂಕ idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_ಮೊದಲ_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	ಶೂನ್ಯ emulate_read_ಮೊದಲ(FfInitVals *initvals);

	Mem(Module *module, IdString memid, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), ಅಗಲ(ಅಗಲ), ಆರಂಭ_ಸ್ಥಾನಾಂತರ(ಆರಂಭ_ಸ್ಥಾನಾಂತರ), ಗಾತ್ರ_2(ಗಾತ್ರ_2) {}
	Mem(Module *module, TwineSpec &&memid, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
			: Mem(module, module->twines().ಕೂಡಿಸು(std::move(memid)), ಅಗಲ, ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
ಸಾರ್ವಜನಿಕ:
	class range; class iterator;
	using addr_t = uint32_t;
ಖಾಸಗಿ:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	ಪೂರ್ಣಾಂಕ addr_ಅಗಲ_2;
	ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ_2;
	RTLIL::Const default_ಮೌಲ್ಯ;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> ಮೌಲ್ಯಗಳು_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ಸ್ಥಿರಾಂಕ;
	addr_t range_ಗಾತ್ರ(std::map<addr_t, RTLIL::Const>::iterator it) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ it->ಎರಡನೇ.ಗಾತ್ರ_2() / ದತ್ತಾಂಶ_ಅಗಲ_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ it->ಮೊದಲ; }
	addr_t range_ಕೊನೆ(std::map<addr_t, RTLIL::Const>::iterator it) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _range_begin(it) + range_ಗಾತ್ರ(it); }
	// check if the iterator points to a range containing addr
	bool range_ಒಳಗೊಂಡಿದೆ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ಸ್ಥಿರಾಂಕ;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_ಒಳಗೊಂಡಿದೆ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ;
	// return the offset the addr would have in the range at `it`
	ಗಾತ್ರ_t range_ಸ್ಥಾನಾಂತರ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (addr - it->ಮೊದಲ) * ದತ್ತಾಂಶ_ಅಗಲ_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_ದತ್ತಾಂಶ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		ಹಿಂತಿರುಗಿ RTLIL::Const::iterator(it->ಎರಡನೇ, range_ಸ್ಥಾನಾಂತರ(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t ಕೊನೆ_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ಸ್ಥಿರಾಂಕ &ದತ್ತಾಂಶ);
ಸಾರ್ವಜನಿಕ:
	class range {
		ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ_2;
		addr_t ತಳ_2;
		RTLIL::Const ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯಗಳು_2;
		ಮಿತ್ರ class iterator;
		range(ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ, addr_t ತಳ, RTLIL::Const ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯಗಳು)
		: ದತ್ತಾಂಶ_ಅಗಲ_2(ದತ್ತಾಂಶ_ಅಗಲ), ತಳ_2(ತಳ), ಮೌಲ್ಯಗಳು_2(ಮೌಲ್ಯಗಳು) {}
	ಸಾರ್ವಜನಿಕ:
		addr_t ತಳ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ತಳ_2; }
		addr_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ((addr_t) ಮೌಲ್ಯಗಳು_2.ಗಾತ್ರ_2()) / ದತ್ತಾಂಶ_ಅಗಲ_2; }
		addr_t ಎಲ್ಲೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ತಳ_2 + ಗಾತ್ರ_2(); }
		RTLIL::Const ಸ್ಥಿರಾಂಕ &concatenated() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2; }
		RTLIL::Const ಕ್ರಿಯಾಕಾರಕ[](addr_t addr) ಸ್ಥಿರಾಂಕ {
			log_assert(addr - ತಳ_2 < ಗಾತ್ರ_2());
			ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.ಹೊರತೆಗೆ((addr - ತಳ_2) * ದತ್ತಾಂಶ_ಅಗಲ_2, ದತ್ತಾಂಶ_ಅಗಲ_2);
		}
		RTLIL::Const at_ಸ್ಥಾನಾಂತರ(addr_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (*this)[ತಳ_2 + ಸ್ಥಾನಾಂತರ]; }
	};
	class iterator {
		MemContents ಸ್ಥಿರಾಂಕ *ಸ್ಮರಣೆ_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		ಮಿತ್ರ class MemContents;
		iterator(MemContents ಸ್ಥಿರಾಂಕ *ಸ್ಮರಣೆ_3, addr_t addr) : ಸ್ಮರಣೆ_2(ಸ್ಮರಣೆ_3), _addr(addr) {}
	ಸಾರ್ವಜನಿಕ:
		using iterator_category = std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್;
		using ಮೌಲ್ಯ_type = range;
		using ಸೂಚಕ = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference ಕ್ರಿಯಾಕಾರಕ *() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ range(ಸ್ಮರಣೆ_2->ದತ್ತಾಂಶ_ಅಗಲ_2, _addr, ಸ್ಮರಣೆ_2->ಮೌಲ್ಯಗಳು_2.at(_addr)); }
		ಸೂಚಕ ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ arrow_proxy<range>(**this); }
		bool ಕ್ರಿಯಾಕಾರಕ !=(iterator ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಮರಣೆ_2 != other.ಸ್ಮರಣೆ_2 || _addr != other._addr; }
		bool ಕ್ರಿಯಾಕಾರಕ ==(iterator ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !(*this != other); }
		iterator &ಕ್ರಿಯಾಕಾರಕ++();
	};
	MemContents(ಪೂರ್ಣಾಂಕ addr_ಅಗಲ, ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ, RTLIL::Const default_ಮೌಲ್ಯ_2)
		: addr_ಅಗಲ_2(addr_ಅಗಲ), ದತ್ತಾಂಶ_ಅಗಲ_2(ದತ್ತಾಂಶ_ಅಗಲ)
		, default_ಮೌಲ್ಯ((default_ಮೌಲ್ಯ_2.extu(ದತ್ತಾಂಶ_ಅಗಲ), std::move(default_ಮೌಲ್ಯ_2)))
	{ log_assert(addr_ಅಗಲ_2 > 0 && addr_ಅಗಲ_2 < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(addr_t) * 8); log_assert(ದತ್ತಾಂಶ_ಅಗಲ_2 > 0); }
	MemContents(ಪೂರ್ಣಾಂಕ addr_ಅಗಲ, ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ) : MemContents(addr_ಅಗಲ, ದತ್ತಾಂಶ_ಅಗಲ, RTLIL::Const(State::Sx, ದತ್ತಾಂಶ_ಅಗಲ)) {}
	explicit MemContents(Mem *mem);
	ಪೂರ್ಣಾಂಕ addr_ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ addr_ಅಗಲ_2; }
	ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_ಅಗಲ_2; }
	RTLIL::Const ಸ್ಥಿರಾಂಕ &default_ಮೌಲ್ಯ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const ಕ್ರಿಯಾಕಾರಕ [](addr_t addr) ಸ್ಥಿರಾಂಕ;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t ಎಣಿಕೆ_range(addr_t begin_addr, addr_t ಕೊನೆ_addr) ಸ್ಥಿರಾಂಕ;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	ಶೂನ್ಯ reserve_range(addr_t begin_addr, addr_t ಕೊನೆ_addr) { _reserve_range(begin_addr, ಕೊನೆ_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	ಶೂನ್ಯ ಸೇರಿಸು_concatenated(addr_t addr, RTLIL::Const ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯಗಳು);
	// insert multiple words at the given address, overriding any previous assignment.
	ಮಾದರಿ<typename Iterator> ಶೂನ್ಯ ಸೇರಿಸು_range(addr_t addr, Iterator begin, Iterator ಕೊನೆ) {
		ಸ್ವಯಂಚಾಲಿತ ಪದಗಳು_2 = ಕೊನೆ - begin;
		log_assert(addr < (addr_t)(1<<addr_ಅಗಲ_2)); log_assert(ಪದಗಳು_2 <= (addr_t)(1<<addr_ಅಗಲ_2) - addr);
		ಸ್ವಯಂಚಾಲಿತ range = _reserve_range(addr, addr + ಪದಗಳು_2);
		ಸ್ವಯಂಚಾಲಿತ it = range_ದತ್ತಾಂಶ(range, addr);
		ಗಾಗಿ(; begin != ಕೊನೆ; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	ಶೂನ್ಯ clear_range(addr_t begin_addr, addr_t ಕೊನೆ_addr);
	// check invariants, abort if invariants failed
	ಶೂನ್ಯ check();
	iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.ಖಾಲಿಗಳು() ? ಕೊನೆ() : iterator(this, ಮೌಲ್ಯಗಳು_2.begin()->ಮೊದಲ); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_2.ಖಾಲಿಗಳು(); }
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
