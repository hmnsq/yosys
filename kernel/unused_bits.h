#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

સંરચના UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, અચલ NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	શૂન્ય collect_used(RTLIL::Module *module, અચલ NewCellTypes &ct)
	{
		માટે (auto cell : module->cells())
			માટે (auto &conn : cell->connections())
				જો (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		માટે (auto wire : module->wires())
			જો (wire->port_output)
				add_used(wire);
	}

	શૂન્ય add_used(અચલ RTLIL::SigSpec &sig)
	{
		માટે (auto bit : sigmap(sig))
			જો (bit.wire != nullptr)
				used.ઉમેરો(bit);
	}

	bool check(RTLIL::SigBit bit) અચલ
	{
		RTLIL::SigBit mapped = sigmap(bit);
		પાછા mapped.wire != nullptr && !used.ગણતરી(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
