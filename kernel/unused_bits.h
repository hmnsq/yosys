#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਅਣਵਰਤਿਆ_ਬਿੱਟ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਣਵਰਤਿਆ_ਬਿੱਟ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ ਅਣਵਰਤਿਆ_ਬਿੱਟ
{
	SigMap sigmap;
	pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਵਰਤਿਆ;

	ਅਣਵਰਤਿਆ_ਬਿੱਟ(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_ਵਰਤਿਆ(module, ct);
	}

	ਅਣਵਰਤਿਆ_ਬਿੱਟ(RTLIL::Module *module, ਅਚਲ NewCellTypes &ct) : sigmap(module)
	{
		collect_ਵਰਤਿਆ(module, ct);
	}

	ਖਾਲੀ collect_ਵਰਤਿਆ(RTLIL::Module *module, ਅਚਲ NewCellTypes &ct)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਜੇ (!ct.cell_output(cell->type, conn.ਪਹਿਲਾ))
					ਜੋੜੋ_ਵਰਤਿਆ(conn.ਦੂਜਾ);

		ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
			ਜੇ (wire->ਪੋਰਟ_output)
				ਜੋੜੋ_ਵਰਤਿਆ(wire);
	}

	ਖਾਲੀ ਜੋੜੋ_ਵਰਤਿਆ(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sigmap(sig))
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != nullptr)
				ਵਰਤਿਆ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	bool check(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ mapped = sigmap(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ mapped.wire != nullptr && !ਵਰਤਿਆ.ਗਿਣਤੀ_2(mapped);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
