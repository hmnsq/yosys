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
		དོན་ལུ (auto cell : module->cells())
			དོན་ལུ (auto &conn : cell->connections())
				གལ་སྲིད (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		དོན་ལུ (auto wire : module->wires())
			གལ་སྲིད (wire->port_output)
				add_used(wire);
	}

	void add_used(const RTLIL::SigSpec &sig)
	{
		དོན་ལུ (auto bit : sigmap(sig))
			གལ་སྲིད (bit.wire != nullptr)
				used.insert(bit);
	}

	bool check(RTLIL::SigBit bit) const
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ལོག mapped.wire != nullptr && !used.count(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
