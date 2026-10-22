#ಅನ್ತರ್ಭಾವ "kernel/twine.h"
#ಅನ್ತರ್ಭಾವ "kernel/hashcons_impl.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

ಮಾದರಿ ರಚನೆ HashConsPool<TwinePool, Twine_ಗಂಟು, IdString>;

std::ಸದಿಶ<Twine_ಗಂಟು> StaticTwines::nodes_;

ಶೂನ್ಯ StaticTwines::init() {
	ಆದರೆ (ready())
		ಹಿಂತಿರುಗಿ;
	log_assert(nodes_.ಖಾಲಿಗಳು());
	nodes_.reserve(STATIC_TWINE_ಕೊನೆ);
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name : ID::static_ಹೆಸರುಗಳು)
		nodes_.emplace_ಹಿಂಭಾಗ(std::string_view(name));
}

ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &StaticTwines::ಗಂಟು(ಗಾತ್ರ_t idx) { ಹಿಂತಿರುಗಿ nodes_[idx]; }
ತಾರ್ಕಿಕ StaticTwines::ready() { ಹಿಂತಿರುಗಿ nodes_.ಗಾತ್ರ_2() == STATIC_TWINE_ಕೊನೆ; }

int64_t twine_gc_ns;
ಪೂರ್ಣಾಂಕ twine_gc_ಎಣಿಕೆ;

std::string IdString::handle_token() ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ stringf("%s@%zu", is_ಬಹಿರಂಗ() ? "$pub" : "$priv", untag().raw());
}

ಗಾತ್ರ_t IdString::handle_token_ಪೂರ್ವಪ್ರತ್ಯಯ(std::string_view token, ತಾರ್ಕಿಕ &is_ಬಹಿರಂಗ_2) {
	ಆದರೆ (token.substr(0, 5) == "$pub@") {
		is_ಬಹಿರಂಗ_2 = ನಿಜ;
		ಹಿಂತಿರುಗಿ 5;
	}
	ಆದರೆ (token.substr(0, 6) == "$priv@") {
		is_ಬಹಿರಂಗ_2 = ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ 6;
	}
	ಹಿಂತಿರುಗಿ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_ಕೊನೆ);
	std::string ಫಲಿತಾಂಶ = ref.is_ಬಹಿರಂಗ() ? "\\" : "";
	ಫಲಿತಾಂಶ += static_ಹೆಸರುಗಳು[idx.raw()];
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_ಕೊನೆ);
	ಹಿಂತಿರುಗಿ static_ಹೆಸರುಗಳು[idx.raw()];
}

TwineSpec::TwineSpec(ಎಲೆ v) : ದತ್ತಾಂಶ(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : ದತ್ತಾಂಶ(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : ದತ್ತಾಂಶ(std::move(v)) {}

ಶೂನ್ಯ SmallString::store(std::string_view content)
{
	log_assert(content.ಗಾತ್ರ_2() <= MAX_LEN);
	release();
	len_ = content.ಗಾತ್ರ_2();
	ಆದರೆ (len_ > INLINE_CAP) {
		ptr_ = ಹೊಸ ಅಕ್ಷರ[len_];
		ಸ್ಮರಣೆ_ನಕಲು(ptr_, content.ದತ್ತಾಂಶ(), len_);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸ್ಮರಣೆ_ನಕಲು(inl_, content.ದತ್ತಾಂಶ(), len_);
	}
}

ಶೂನ್ಯ SmallString::release()
{
	ಆದರೆ (len_ > INLINE_CAP)
		ಅಳಿಸು[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ಸ್ಥಿರಾಂಕ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	ಸ್ಮರಣೆ_ನಕಲು(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ SmallString &other)
{
	ಆದರೆ (ಇದು != &other)
		store(other.view());
	ಹಿಂತಿರುಗಿ *ಇದು;
}

SmallString &SmallString::ಕ್ರಿಯಾಕಾರಕ=(SmallString &&other) noexcept
{
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ *ಇದು;
	release();
	ಸ್ಮರಣೆ_ನಕಲು(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ಹಿಂತಿರುಗಿ *ಇದು;
}

SmallString::~SmallString() { release(); }

Twine_ಗಂಟು::Twine_ಗಂಟು(Twine_ಗಂಟು &&other) noexcept
	: ಪಠ್ಯ_2(std::move(other.ಪಠ್ಯ_2)), ಪೂರ್ವಪ್ರತ್ಯಯ_3(other.ಪೂರ್ವಪ್ರತ್ಯಯ_3), hash_(other.hash_)
{
	other.ಪೂರ್ವಪ್ರತ್ಯಯ_3 = DEAD;
}

Twine_ಗಂಟು &Twine_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ=(Twine_ಗಂಟು &&other) noexcept
{
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ *ಇದು;
	ಪಠ್ಯ_2 = std::move(other.ಪಠ್ಯ_2);
	ಪೂರ್ವಪ್ರತ್ಯಯ_3 = other.ಪೂರ್ವಪ್ರತ್ಯಯ_3;
	hash_ = other.hash_;
	other.ಪೂರ್ವಪ್ರತ್ಯಯ_3 = DEAD;
	ಹಿಂತಿರುಗಿ *ಇದು;
}

std::string TwineSpec::content_str() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ *ಎಲೆ_2 = std::get_if<ಎಲೆ>(&ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ ಎಲೆ_2->s;
	ಸ್ವಯಂಚಾಲಿತ &autosfx = std::get<AutoSuffix>(ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ *autosfx.ಪೂರ್ವಪ್ರತ್ಯಯ_2 + autosfx.ಬಾಲ;
}

ಸ್ಥಿರ std::pair<std::string_view, ತಾರ್ಕಿಕ> unescape_name(std::string_view name) {
	ಆದರೆ (!name.ಖಾಲಿಗಳು() && name[0] == '\\')
		ಹಿಂತಿರುಗಿ {name.substr(1), ನಿಜ};
	ಹಿಂತಿರುಗಿ {name, ಸುಳ್ಳು};
}

ಸ್ಥಿರ ಶೂನ್ಯ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	ಗಾಗಿ (ಅಕ್ಷರ ch : name)
		ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)ch <= (ಚಿಹ್ನೆರಹಿತ)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(ಮುಂದಿನ_serial()) {}
TwinePool::TwinePool(ಸ್ಥಿರಾಂಕ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(ಮುಂದಿನ_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(ಮುಂದಿನ_serial()) {}

TwinePool& TwinePool::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ TwinePool& other) {
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ *ಇದು;
	HashConsPool::ಕ್ರಿಯಾಕಾರಕ=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = ಮುಂದಿನ_serial();
	ಹಿಂತಿರುಗಿ *ಇದು;
}

TwinePool& TwinePool::ಕ್ರಿಯಾಕಾರಕ=(TwinePool&& other) {
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ *ಇದು;
	HashConsPool::ಕ್ರಿಯಾಕಾರಕ=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = ಮುಂದಿನ_serial();
	ಹಿಂತಿರುಗಿ *ಇದು;
}

ತಾರ್ಕಿಕ TwinePool::owns(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ref == IdString::Null || ID::is_static(ref))
		ಹಿಂತಿರುಗಿ ref;
	ಹಿಂತಿರುಗಿ ref.stamped(serial_);
}

ಶೂನ್ಯ TwinePool::check_owned(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& TwinePool::ಕ್ರಿಯಾಕಾರಕ[](IdString ref) ಸ್ಥಿರಾಂಕ {
	check_owned(ref);
	ಹಿಂತಿರುಗಿ HashConsPool::ಕ್ರಿಯಾಕಾರಕ[](ref);
}

ಗಾತ್ರ_t TwinePool::gc(pool<IdString>& roots) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : auto_prefixes)
		roots.ಸೇರಿಸು(it.ಎರಡನೇ);
	ಹಿಂತಿರುಗಿ HashConsPool::gc(roots);
}

ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& TwinePool::static_ಗಂಟು(ಗಾತ್ರ_t idx) { ಹಿಂತಿರುಗಿ StaticTwines::ಗಂಟು(idx); }
ಶೂನ್ಯ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

ಗಾತ್ರ_t TwinePool::ಮುಂದಿನ_serial() {
	ಸ್ಥಿರ ಗಾತ್ರ_t counter = 0;
	ಗಾತ್ರ_t serial = ++counter;
	ಆದರೆ constexpr (IdString::MAX_SERIAL == 0)
		ಹಿಂತಿರುಗಿ serial;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::ಕೂಡಿಸು_inner(Twine_ಗಂಟು t) {
	ಆದರೆ (free_list.ಖಾಲಿಗಳು() && STATIC_ಎಣಿಕೆ + backing.ಗಾತ್ರ_2() > IdString::MAX_ಸೂಚ್ಯಂಕ)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_ಸೂಚ್ಯಂಕ - STATIC_ಎಣಿಕೆ);
	ಹಿಂತಿರುಗಿ HashConsPool::ಕೂಡಿಸು_inner(std::move(t));
}

ಶೂನ್ಯ TwinePool::dump(IdString ref, std::ostream& os) ಸ್ಥಿರಾಂಕ {
	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& twine = (*ಇದು)[ref];
	ಆಯ್ಕೆ (twine.ಬಗೆ()) {
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Dead:
		os << "Dead()";
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::ಎಲೆ:
		os << "Leaf(\"" << twine.ಪಠ್ಯ() << "\")";
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.ಪೂರ್ವಪ್ರತ್ಯಯ_2(), os);
		os << ", tail: \"" << twine.ಪಠ್ಯ() << "\")";
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ref.is_ಬಹಿರಂಗ())
		os << " pub";
}

ಶೂನ್ಯ TwinePool::append_str(IdString ref, std::string& ಹೊರಹೋಗು) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ref == IdString::Null)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (ref.is_ಬಹಿರಂಗ())
		ಹೊರಹೋಗು += '\\';
	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& twine = (*ಇದು)[ref];
	ಆಯ್ಕೆ (twine.ಬಗೆ()) {
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Dead:
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Suffix:
		append_str(twine.ಪೂರ್ವಪ್ರತ್ಯಯ_2(), ಹೊರಹೋಗು);
		[[fallthrough]];
	ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::ಎಲೆ:
		ಹೊರಹೋಗು += twine.ಪಠ್ಯ();
		ನಿಲ್ಲಿಸು;
	}
}

std::string TwinePool::str(IdString ref) ಸ್ಥಿರಾಂಕ {
	std::string ಹೊರಹೋಗು;
	append_str(ref, ಹೊರಹೋಗು);
	ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು;
}

std::string TwinePool::unescaped_str(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ) ಸ್ಥಿರಾಂಕ {
	uint32_t hash = ಪೂರ್ವಪ್ರತ್ಯಯ_2 == Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ ? 0 : (*ಇದು)[IdString(ಪೂರ್ವಪ್ರತ್ಯಯ_2)].hash();
	ಹಿಂತಿರುಗಿ Twine_ಗಂಟು::extend_hash(hash, ಪಠ್ಯ);
}

IdString TwinePool::ಕಂಡುಹಿಡಿ_content(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_content(ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ, content_hash(ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ));
}

IdString TwinePool::intern(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ) {
	log_assert(!Multithreading::ಸಕ್ರಿಯ());
	uint32_t hash = content_hash(ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ);
	IdString ref = ಕಂಡುಹಿಡಿ_content(ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ, hash);
	ಆದರೆ (ref != IdString::Null)
		ಹಿಂತಿರುಗಿ ref;
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_inner(Twine_ಗಂಟು{ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ, hash});
}

IdString TwinePool::ಕಂಡುಹಿಡಿ(std::string_view name) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (name.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ IdString::Null;
	check_name(name);
	ಸ್ವಯಂಚಾಲಿತ [content, is_ಬಹಿರಂಗ_2] = unescape_name(name);
	ಹಿಂತಿರುಗಿ stamp(ಕಂಡುಹಿಡಿ_content(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, content).ಟ್ಯಾಗ್(is_ಬಹಿರಂಗ_2));
}

IdString TwinePool::ಕಂಡುಹಿಡಿ(TwineSpec t) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (std::holds_alternative<TwineSpec::AutoSuffix>(t.ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ stamp(ಕಂಡುಹಿಡಿ_content(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, t.content_str()));
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ *ಎಲೆ_2 = std::get_if<TwineSpec::ಎಲೆ>(&t.ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ stamp(ಕಂಡುಹಿಡಿ_content(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, ಎಲೆ_2->s));
	ಸ್ಥಿರಾಂಕ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ stamp(ಕಂಡುಹಿಡಿ_content((uint32_t)sfx.ಪೂರ್ವಪ್ರತ್ಯಯ_2.untag().raw(), sfx.ಬಾಲ).ಟ್ಯಾಗ್(sfx.ಪೂರ್ವಪ್ರತ್ಯಯ_2.is_ಬಹಿರಂಗ()));
}

IdString TwinePool::ಕೂಡಿಸು(TwineSpec t) {
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ *ap = std::get_if<TwineSpec::AutoSuffix>(&t.ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(auto_ಪೂರ್ವಪ್ರತ್ಯಯ(ap->ಪೂರ್ವಪ್ರತ್ಯಯ_2), ap->ಬಾಲ);
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ *ಎಲೆ_2 = std::get_if<TwineSpec::ಎಲೆ>(&t.ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ stamp(intern(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, ಎಲೆ_2->s));
	ಸ್ಥಿರಾಂಕ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(sfx.ಪೂರ್ವಪ್ರತ್ಯಯ_2, sfx.ಬಾಲ);
}

IdString TwinePool::auto_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ std::string *ಪೂರ್ವಪ್ರತ್ಯಯ_2) {
	ಸ್ವಯಂಚಾಲಿತ it = auto_prefixes.ಕಂಡುಹಿಡಿ(ಪೂರ್ವಪ್ರತ್ಯಯ_2);
	ಆದರೆ (it != auto_prefixes.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
	IdString ref = intern(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, *ಪೂರ್ವಪ್ರತ್ಯಯ_2);
	auto_prefixes[ಪೂರ್ವಪ್ರತ್ಯಯ_2] = ref;
	ಹಿಂತಿರುಗಿ ref;
}

IdString TwinePool::ಕೂಡಿಸು(IdString ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಬಾಲ) {
	ಹಿಂತಿರುಗಿ stamp(intern((uint32_t)ಪೂರ್ವಪ್ರತ್ಯಯ_2.untag().raw(), ಬಾಲ).ಟ್ಯಾಗ್(ಪೂರ್ವಪ್ರತ್ಯಯ_2.is_ಬಹಿರಂಗ()));
}

IdString TwinePool::ಕೂಡಿಸು(std::string_view s) {
	ಆದರೆ (s.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ IdString::Null;
	check_name(s);
	ಸ್ವಯಂಚಾಲಿತ [content, is_ಬಹಿರಂಗ_2] = unescape_name(s);
	ಹಿಂತಿರುಗಿ stamp(intern(Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ, content).ಟ್ಯಾಗ್(is_ಬಹಿರಂಗ_2));
}

IdString TwinePool::copy_from(ಸ್ಥಿರಾಂಕ TwinePool& src, IdString ref) {
	IdString ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_from(src, ref);
	ಆದರೆ (ಸಿಕ್ಕಿತು != IdString::Null || ref == IdString::Null)
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& t = src[ref.untag()];
	ಆದರೆ (t.is_dead())
		ಹಿಂತಿರುಗಿ IdString::Null;
	uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2 = Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಆದರೆ (t.is_suffix())
		ಪೂರ್ವಪ್ರತ್ಯಯ_2 = (uint32_t)copy_from(src, t.ಪೂರ್ವಪ್ರತ್ಯಯ_2()).untag().raw();
	ಹಿಂತಿರುಗಿ stamp(intern(ಪೂರ್ವಪ್ರತ್ಯಯ_2, t.ಪಠ್ಯ()).ಟ್ಯಾಗ್(ref.is_ಬಹಿರಂಗ()));
}

std::string TwinePool::ref_token(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ "#" + std::ಗಮ್ಯ_string((uint64_t)serial_) + ":"
		+ std::ಗಮ್ಯ_string((uint64_t)stamp(ref).ಬಿಟ್ಟುಗಳು());
}

IdString TwinePool::ref_from_token(std::string_view token) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (token.ಗಾತ್ರ_2() < 2 || token[0] != '#')
		ಹಿಂತಿರುಗಿ IdString::Null;
	ಗಾತ್ರ_t sep = token.ಕಂಡುಹಿಡಿ(':');
	ಆದರೆ (sep == std::string_view::npos)
		ಹಿಂತಿರುಗಿ IdString::Null;
	ಗಾತ್ರ_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 2; i++) {
		ಆದರೆ (parts[i].ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ IdString::Null;
		ಗಾಗಿ (ಅಕ್ಷರ c : parts[i]) {
			ಆದರೆ (c < '0' || c > '9')
				ಹಿಂತಿರುಗಿ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ಆದರೆ (fields[0] != serial_)
		ಹಿಂತಿರುಗಿ IdString::Null;
	IdString ref(fields[1]);
	ಆದರೆ (ref == IdString::Null)
		ಹಿಂತಿರುಗಿ IdString::Null;
	ಹಿಂತಿರುಗಿ is_ಜೀವಂತ(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ರಚನೆ TwineSegments {
	ಸ್ಥಿರ constexpr ಗಾತ್ರ_t SEGMENT_INLINE_ಆಳ = 8;
	TwineSegments(ಸ್ಥಿರಾಂಕ TwinePool &pool, IdString ref);
	TwineSegments(ಸ್ಥಿರಾಂಕ TwinePool &pool, IdString ತಲೆ, std::string_view ಬಾಲ);
	std::string_view peek();
	ಶೂನ್ಯ advance(ಗಾತ್ರ_t n);
	ಗಾತ್ರ_t ಒಟ್ಟು_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ;
ಖಾಸಗಿ:
	ಶೂನ್ಯ ತಳ್ಳು(std::string_view seg);
	std::string_view *segs();
	ಸ್ಥಿರಾಂಕ std::string_view *segs() ಸ್ಥಿರಾಂಕ;
	std::string_view inline_segs[SEGMENT_INLINE_ಆಳ];
	std::ಸದಿಶ<std::string_view> spill;
	ಗಾತ್ರ_t ಎಣಿಕೆ_2 = 0;
	ಗಾತ್ರ_t pos = 0;
	ಗಾತ್ರ_t off = 0;
};

TwineSegments::TwineSegments(ಸ್ಥಿರಾಂಕ TwinePool &pool, IdString ref)
{
	ಆದರೆ (ref == IdString::Null)
		ಹಿಂತಿರುಗಿ;
	ಗಾಗಿ (IdString cur = ref.untag(); ;) {
		ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &t = pool[cur];
		ಆಯ್ಕೆ (t.ಬಗೆ()) {
		ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Suffix:
			ತಳ್ಳು(t.ಪಠ್ಯ());
			cur = t.ಪೂರ್ವಪ್ರತ್ಯಯ_2();
			ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::ಎಲೆ:
			ತಳ್ಳು(t.ಪಠ್ಯ());
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ Twine_ಗಂಟು::ಬಗೆ_2::Dead:
			ನಿಲ್ಲಿಸು;
		}
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ref.is_ಬಹಿರಂಗ())
		ತಳ್ಳು("\\");
	std::ಹಿಮ್ಮುಖ(segs(), segs() + ಎಣಿಕೆ_2);
}

TwineSegments::TwineSegments(ಸ್ಥಿರಾಂಕ TwinePool &pool, IdString ತಲೆ, std::string_view ಬಾಲ)
	: TwineSegments(pool, ತಲೆ)
{
	ತಳ್ಳು(ಬಾಲ);
}

std::string_view TwineSegments::peek()
{
	ಆಗ (pos < ಎಣಿಕೆ_2) {
		std::string_view seg = segs()[pos];
		ಆದರೆ (off < seg.ಗಾತ್ರ_2())
			ಹಿಂತಿರುಗಿ seg.substr(off);
		pos++;
		off = 0;
	}
	ಹಿಂತಿರುಗಿ {};
}

ಶೂನ್ಯ TwineSegments::advance(ಗಾತ್ರ_t n) { off += n; }

ಗಾತ್ರ_t TwineSegments::ಒಟ್ಟು_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
	ಗಾತ್ರ_t ಒಟ್ಟು = 0;
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < ಎಣಿಕೆ_2; i++)
		ಒಟ್ಟು += segs()[i].ಗಾತ್ರ_2();
	ಹಿಂತಿರುಗಿ ಒಟ್ಟು;
}

ಶೂನ್ಯ TwineSegments::ತಳ್ಳು(std::string_view seg)
{
	ಆದರೆ (spill.ಖಾಲಿಗಳು() && ಎಣಿಕೆ_2 < SEGMENT_INLINE_ಆಳ) {
		inline_segs[ಎಣಿಕೆ_2++] = seg;
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (spill.ಖಾಲಿಗಳು())
		spill.ನಿಯೋಜಿಸು(inline_segs, inline_segs + ಎಣಿಕೆ_2);
	spill.ತಳ್ಳು_ಹಿಂಭಾಗ(seg);
	ಎಣಿಕೆ_2++;
}

std::string_view *TwineSegments::segs() { ಹಿಂತಿರುಗಿ spill.ಖಾಲಿಗಳು() ? inline_segs : spill.ದತ್ತಾಂಶ(); }
ಸ್ಥಿರಾಂಕ std::string_view *TwineSegments::segs() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ spill.ಖಾಲಿಗಳು() ? inline_segs : spill.ದತ್ತಾಂಶ(); }

ಗಾತ್ರ_t TwinePool::str_ಗಾತ್ರ(IdString ref) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ TwineSegments(*ಇದು, ref).ಒಟ್ಟು_ಗಾತ್ರ();
}

ತಾರ್ಕಿಕ TwinePool::begins_with(IdString ref, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2) ಸ್ಥಿರಾಂಕ
{
	TwineSegments segs(*ಇದು, ref);
	ಆಗ (!ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಖಾಲಿಗಳು()) {
		std::string_view seg = segs.peek();
		ಆದರೆ (seg.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಗಾತ್ರ_t n = std::min(seg.ಗಾತ್ರ_2(), ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಗಾತ್ರ_2());
		ಆದರೆ (std::memcmp(seg.ದತ್ತಾಂಶ(), ಪೂರ್ವಪ್ರತ್ಯಯ_2.ದತ್ತಾಂಶ(), n) != 0)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		segs.advance(n);
		ಪೂರ್ವಪ್ರತ್ಯಯ_2.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(n);
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ TwinePool::name_equal(IdString ref, std::string_view name) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ str_ಗಾತ್ರ(ref) == name.ಗಾತ್ರ_2() && begins_with(ref, name);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ಆಗ (ನಿಜ) {
		std::string_view x = sa.peek(), y = sb.peek();
		ಆದರೆ (x.ಖಾಲಿಗಳು() || y.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ x.ಖಾಲಿಗಳು() ? (y.ಖಾಲಿಗಳು() ? 0 : -1) : 1;
		ಗಾತ್ರ_t n = std::min(x.ಗಾತ್ರ_2(), y.ಗಾತ್ರ_2());
		ಆದರೆ (ಪೂರ್ಣಾಂಕ diff = std::memcmp(x.ದತ್ತಾಂಶ(), y.ದತ್ತಾಂಶ(), n); diff != 0)
			ಹಿಂತಿರುಗಿ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::ಕಂಡುಹಿಡಿ_content(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ, uint32_t hash) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_hashed(hash, [&](IdString ref) {
		ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &t = (*ಇದು)[ref];
		ಆದರೆ (t == Twine_ಗಂಟು::ಕೀಲಿ_2{ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪಠ್ಯ})
			ಹಿಂತಿರುಗಿ ನಿಜ;
		IdString ತಲೆ = ಪೂರ್ವಪ್ರತ್ಯಯ_2 == Twine_ಗಂಟು::NO_ಪೂರ್ವಪ್ರತ್ಯಯ ? IdString::Null : IdString(ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		TwineSegments sa(*ಇದು, ref), sb(*ಇದು, ತಲೆ, ಪಠ್ಯ);
		ಹಿಂತಿರುಗಿ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::ಕಂಡುಹಿಡಿ_from(ಸ್ಥಿರಾಂಕ TwinePool& src, IdString ref) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ref == IdString::Null || ID::is_static(ref))
		ಹಿಂತಿರುಗಿ ref;
	IdString idx = ref.untag();
	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& t = src[idx];
	ಆದರೆ (t.is_dead())
		ಹಿಂತಿರುಗಿ IdString::Null;
	IdString ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ಇದು, cand), sb(src, idx);
		ಹಿಂತಿರುಗಿ compare_segments(sa, sb) == 0;
	});
	ಹಿಂತಿರುಗಿ stamp(ಸಿಕ್ಕಿತು.ಟ್ಯಾಗ್(ref.is_ಬಹಿರಂಗ()));
}

IdString TwinePool::ಪೂರ್ವಪ್ರತ್ಯಯ_ದ(IdString ref) ಸ್ಥಿರಾಂಕ {
	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &t = (*ಇದು)[ref];
	ಆದರೆ (!t.is_suffix())
		ಹಿಂತಿರುಗಿ IdString::Null;
	ಹಿಂತಿರುಗಿ stamp(t.ಪೂರ್ವಪ್ರತ್ಯಯ_2().ಟ್ಯಾಗ್(ref.is_ಬಹಿರಂಗ()));
}

ಪೂರ್ಣಾಂಕ TwinePool::compare_by_name(IdString a, IdString b) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (a == b)
		ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (a == IdString::Null)
		ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (b == IdString::Null)
		ಹಿಂತಿರುಗಿ 1;

	ಆದರೆ (a.is_ಬಹಿರಂಗ() == b.is_ಬಹಿರಂಗ()) {
		ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &ta = (*ಇದು)[a.untag()];
		ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &tb = (*ಇದು)[b.untag()];
		ಆದರೆ (ta.is_ಎಲೆ() && tb.is_ಎಲೆ())
			ಹಿಂತಿರುಗಿ ta.ಪಠ್ಯ().compare(tb.ಪಠ್ಯ());
		ಆದರೆ (ta.is_suffix() && tb.is_suffix() && ta.ಪೂರ್ವಪ್ರತ್ಯಯ_2() == tb.ಪೂರ್ವಪ್ರತ್ಯಯ_2())
			ಹಿಂತಿರುಗಿ ta.ಪಠ್ಯ().compare(tb.ಪಠ್ಯ());
	}

	TwineSegments sa(*ಇದು, a), sb(*ಇದು, b);
	ಹಿಂತಿರುಗಿ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_ಕೊನೆ
