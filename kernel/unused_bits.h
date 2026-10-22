#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, const NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	void collect_used(RTLIL::Module *module, const NewCellTypes &ct)
	{
		ꦏꦁꦒꦺꦴ (auto cell : module->cells())
			ꦏꦁꦒꦺꦴ (auto &conn : cell->connections())
				ꦪꦼꦤ꧀ (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		ꦏꦁꦒꦺꦴ (auto wire : module->wires())
			ꦪꦼꦤ꧀ (wire->port_output)
				add_used(wire);
	}

	void add_used(const RTLIL::SigSpec &sig)
	{
		ꦏꦁꦒꦺꦴ (auto bit : sigmap(sig))
			ꦪꦼꦤ꧀ (bit.wire != nullptr)
				used.insert(bit);
	}

	bool check(RTLIL::SigBit bit) const
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ꦮꦭꦶ mapped.wire != nullptr && !used.count(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
