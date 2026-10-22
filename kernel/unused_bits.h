#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ഘടന UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, സ്ഥിരം NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	ശൂന്യം collect_used(RTLIL::Module *module, സ്ഥിരം NewCellTypes &ct)
	{
		വേണ്ടി (auto cell : module->cells())
			വേണ്ടി (auto &conn : cell->connections())
				എങ്കിൽ (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		വേണ്ടി (auto wire : module->wires())
			എങ്കിൽ (wire->port_output)
				add_used(wire);
	}

	ശൂന്യം add_used(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (auto bit : sigmap(sig))
			എങ്കിൽ (bit.wire != nullptr)
				used.തിരുകുക(bit);
	}

	bool check(RTLIL::SigBit bit) സ്ഥിരം
	{
		RTLIL::SigBit mapped = sigmap(bit);
		തിരികെ mapped.wire != nullptr && !used.എണ്ണം(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
