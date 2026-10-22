#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/twine.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/hashcons_impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ ਬਣਤਰ HashConsPool<TwinePool, Twine_ਗੰਢ, IdString>;

std::ਸਦਿਸ਼<Twine_ਗੰਢ> StaticTwines::nodes_;

ਖਾਲੀ StaticTwines::init() {
	ਜੇ (ready())
		ਵਾਪਸ;
	log_assert(nodes_.ਖਾਲੀ_ਥਾਵਾਂ());
	nodes_.reserve(STATIC_TWINE_ਸਿਰਾ);
	ਲਈ (ਅਚਲ ਅੱਖਰ *name : ID::static_ਨਾਂ)
		nodes_.emplace_ਪਿੱਠ(std::string_view(name));
}

ਅਚਲ Twine_ਗੰਢ &StaticTwines::ਗੰਢ(ਮਾਪ_t idx) { ਵਾਪਸ nodes_[idx]; }
ਤਾਰਕਿਕ StaticTwines::ready() { ਵਾਪਸ nodes_.ਮਾਪ() == STATIC_TWINE_ਸਿਰਾ; }

int64_t twine_gc_ns;
ਪੂਰਨ twine_gc_ਗਿਣਤੀ;

std::string IdString::handle_token() ਅਚਲ {
	ਵਾਪਸ stringf("%s@%zu", is_ਜਨਤਕ() ? "$pub" : "$priv", untag().raw());
}

ਮਾਪ_t IdString::handle_token_ਅਗੇਤਰ(std::string_view token, ਤਾਰਕਿਕ &is_ਜਨਤਕ_2) {
	ਜੇ (token.substr(0, 5) == "$pub@") {
		is_ਜਨਤਕ_2 = ਸੱਚ;
		ਵਾਪਸ 5;
	}
	ਜੇ (token.substr(0, 6) == "$priv@") {
		is_ਜਨਤਕ_2 = ਝੂਠ;
		ਵਾਪਸ 6;
	}
	ਵਾਪਸ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_ਸਿਰਾ);
	std::string ਨਤੀਜਾ = ref.is_ਜਨਤਕ() ? "\\" : "";
	ਨਤੀਜਾ += static_ਨਾਂ[idx.raw()];
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_ਸਿਰਾ);
	ਵਾਪਸ static_ਨਾਂ[idx.raw()];
}

TwineSpec::TwineSpec(ਪੱਤੀ v) : ਅੰਕੜੇ(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : ਅੰਕੜੇ(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : ਅੰਕੜੇ(std::move(v)) {}

ਖਾਲੀ SmallString::store(std::string_view content)
{
	log_assert(content.ਮਾਪ() <= MAX_LEN);
	release();
	len_ = content.ਮਾਪ();
	ਜੇ (len_ > INLINE_CAP) {
		ptr_ = ਨਵਾਂ ਅੱਖਰ[len_];
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr_, content.ਅੰਕੜੇ(), len_);
	} ਨਹੀਂਤਾਂ {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(inl_, content.ਅੰਕੜੇ(), len_);
	}
}

ਖਾਲੀ SmallString::release()
{
	ਜੇ (len_ > INLINE_CAP)
		ਮਿਟਾਓ[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ਅਚਲ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::ਸੰਚਾਲਕ=(ਅਚਲ SmallString &other)
{
	ਜੇ (ਇਹ != &other)
		store(other.view());
	ਵਾਪਸ *ਇਹ;
}

SmallString &SmallString::ਸੰਚਾਲਕ=(SmallString &&other) noexcept
{
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ *ਇਹ;
	release();
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ਵਾਪਸ *ਇਹ;
}

SmallString::~SmallString() { release(); }

Twine_ਗੰਢ::Twine_ਗੰਢ(Twine_ਗੰਢ &&other) noexcept
	: ਪਾਠ_2(std::move(other.ਪਾਠ_2)), ਅਗੇਤਰ_3(other.ਅਗੇਤਰ_3), hash_(other.hash_)
{
	other.ਅਗੇਤਰ_3 = DEAD;
}

Twine_ਗੰਢ &Twine_ਗੰਢ::ਸੰਚਾਲਕ=(Twine_ਗੰਢ &&other) noexcept
{
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ *ਇਹ;
	ਪਾਠ_2 = std::move(other.ਪਾਠ_2);
	ਅਗੇਤਰ_3 = other.ਅਗੇਤਰ_3;
	hash_ = other.hash_;
	other.ਅਗੇਤਰ_3 = DEAD;
	ਵਾਪਸ *ਇਹ;
}

std::string TwineSpec::content_str() ਅਚਲ {
	ਜੇ (ਸਵੈਚਾਲਿਤ *ਪੱਤੀ_2 = std::get_if<ਪੱਤੀ>(&ਅੰਕੜੇ))
		ਵਾਪਸ ਪੱਤੀ_2->s;
	ਸਵੈਚਾਲਿਤ &autosfx = std::get<AutoSuffix>(ਅੰਕੜੇ);
	ਵਾਪਸ *autosfx.ਅਗੇਤਰ_2 + autosfx.ਪੂਛ;
}

ਸਥਿਰ std::pair<std::string_view, ਤਾਰਕਿਕ> unescape_name(std::string_view name) {
	ਜੇ (!name.ਖਾਲੀ_ਥਾਵਾਂ() && name[0] == '\\')
		ਵਾਪਸ {name.substr(1), ਸੱਚ};
	ਵਾਪਸ {name, ਝੂਠ};
}

ਸਥਿਰ ਖਾਲੀ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	ਲਈ (ਅੱਖਰ ch : name)
		ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)ch <= (ਬਿਨਾਨਿਸ਼ਾਨੀ)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(ਅਗਲਾ_serial()) {}
TwinePool::TwinePool(ਅਚਲ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(ਅਗਲਾ_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(ਅਗਲਾ_serial()) {}

TwinePool& TwinePool::ਸੰਚਾਲਕ=(ਅਚਲ TwinePool& other) {
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ *ਇਹ;
	HashConsPool::ਸੰਚਾਲਕ=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = ਅਗਲਾ_serial();
	ਵਾਪਸ *ਇਹ;
}

TwinePool& TwinePool::ਸੰਚਾਲਕ=(TwinePool&& other) {
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ *ਇਹ;
	HashConsPool::ਸੰਚਾਲਕ=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = ਅਗਲਾ_serial();
	ਵਾਪਸ *ਇਹ;
}

ਤਾਰਕਿਕ TwinePool::owns(IdString ref) ਅਚਲ {
	ਵਾਪਸ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ਅਚਲ {
	ਜੇ (ref == IdString::Null || ID::is_static(ref))
		ਵਾਪਸ ref;
	ਵਾਪਸ ref.stamped(serial_);
}

ਖਾਲੀ TwinePool::check_owned(IdString ref) ਅਚਲ {
	ਜੇ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ਅਚਲ Twine_ਗੰਢ& TwinePool::ਸੰਚਾਲਕ[](IdString ref) ਅਚਲ {
	check_owned(ref);
	ਵਾਪਸ HashConsPool::ਸੰਚਾਲਕ[](ref);
}

ਮਾਪ_t TwinePool::gc(pool<IdString>& roots) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : auto_prefixes)
		roots.ਪਾਓ(it.ਦੂਜਾ);
	ਵਾਪਸ HashConsPool::gc(roots);
}

ਅਚਲ Twine_ਗੰਢ& TwinePool::static_ਗੰਢ(ਮਾਪ_t idx) { ਵਾਪਸ StaticTwines::ਗੰਢ(idx); }
ਖਾਲੀ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

ਮਾਪ_t TwinePool::ਅਗਲਾ_serial() {
	ਸਥਿਰ ਮਾਪ_t counter = 0;
	ਮਾਪ_t serial = ++counter;
	ਜੇ constexpr (IdString::MAX_SERIAL == 0)
		ਵਾਪਸ serial;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::ਜੋੜੋ_inner(Twine_ਗੰਢ t) {
	ਜੇ (free_list.ਖਾਲੀ_ਥਾਵਾਂ() && STATIC_ਗਿਣਤੀ + backing.ਮਾਪ() > IdString::MAX_ਸੂਚਕਾਂਕ)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_ਸੂਚਕਾਂਕ - STATIC_ਗਿਣਤੀ);
	ਵਾਪਸ HashConsPool::ਜੋੜੋ_inner(std::move(t));
}

ਖਾਲੀ TwinePool::dump(IdString ref, std::ostream& os) ਅਚਲ {
	ਅਚਲ Twine_ਗੰਢ& twine = (*ਇਹ)[ref];
	ਚੋਣ (twine.ਕਿਸਮ()) {
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Dead:
		os << "Dead()";
		ਰੋਕੋ;
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::ਪੱਤੀ:
		os << "Leaf(\"" << twine.ਪਾਠ() << "\")";
		ਰੋਕੋ;
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.ਅਗੇਤਰ_2(), os);
		os << ", tail: \"" << twine.ਪਾਠ() << "\")";
		ਰੋਕੋ;
	}
	ਜੇ (ref.is_ਜਨਤਕ())
		os << " pub";
}

ਖਾਲੀ TwinePool::append_str(IdString ref, std::string& ਬਾਹਰਲਾ) ਅਚਲ {
	ਜੇ (ref == IdString::Null)
		ਵਾਪਸ;
	ਜੇ (ref.is_ਜਨਤਕ())
		ਬਾਹਰਲਾ += '\\';
	ਅਚਲ Twine_ਗੰਢ& twine = (*ਇਹ)[ref];
	ਚੋਣ (twine.ਕਿਸਮ()) {
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Dead:
		ਰੋਕੋ;
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Suffix:
		append_str(twine.ਅਗੇਤਰ_2(), ਬਾਹਰਲਾ);
		[[fallthrough]];
	ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::ਪੱਤੀ:
		ਬਾਹਰਲਾ += twine.ਪਾਠ();
		ਰੋਕੋ;
	}
}

std::string TwinePool::str(IdString ref) ਅਚਲ {
	std::string ਬਾਹਰਲਾ;
	append_str(ref, ਬਾਹਰਲਾ);
	ਵਾਪਸ ਬਾਹਰਲਾ;
}

std::string TwinePool::unescaped_str(IdString ref) ਅਚਲ {
	ਵਾਪਸ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ) ਅਚਲ {
	uint32_t hash = ਅਗੇਤਰ_2 == Twine_ਗੰਢ::NO_ਅਗੇਤਰ ? 0 : (*ਇਹ)[IdString(ਅਗੇਤਰ_2)].hash();
	ਵਾਪਸ Twine_ਗੰਢ::extend_hash(hash, ਪਾਠ);
}

IdString TwinePool::ਲੱਭੋ_content(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ) ਅਚਲ {
	ਵਾਪਸ ਲੱਭੋ_content(ਅਗੇਤਰ_2, ਪਾਠ, content_hash(ਅਗੇਤਰ_2, ਪਾਠ));
}

IdString TwinePool::intern(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ) {
	log_assert(!Multithreading::ਸਰਗਰਮ());
	uint32_t hash = content_hash(ਅਗੇਤਰ_2, ਪਾਠ);
	IdString ref = ਲੱਭੋ_content(ਅਗੇਤਰ_2, ਪਾਠ, hash);
	ਜੇ (ref != IdString::Null)
		ਵਾਪਸ ref;
	ਵਾਪਸ ਜੋੜੋ_inner(Twine_ਗੰਢ{ਅਗੇਤਰ_2, ਪਾਠ, hash});
}

IdString TwinePool::ਲੱਭੋ(std::string_view name) ਅਚਲ {
	ਜੇ (name.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ IdString::Null;
	check_name(name);
	ਸਵੈਚਾਲਿਤ [content, is_ਜਨਤਕ_2] = unescape_name(name);
	ਵਾਪਸ stamp(ਲੱਭੋ_content(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, content).ਟੈਗ(is_ਜਨਤਕ_2));
}

IdString TwinePool::ਲੱਭੋ(TwineSpec t) ਅਚਲ {
	ਜੇ (std::holds_alternative<TwineSpec::AutoSuffix>(t.ਅੰਕੜੇ))
		ਵਾਪਸ stamp(ਲੱਭੋ_content(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, t.content_str()));
	ਜੇ (ਸਵੈਚਾਲਿਤ *ਪੱਤੀ_2 = std::get_if<TwineSpec::ਪੱਤੀ>(&t.ਅੰਕੜੇ))
		ਵਾਪਸ stamp(ਲੱਭੋ_content(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, ਪੱਤੀ_2->s));
	ਅਚਲ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ਅੰਕੜੇ);
	ਵਾਪਸ stamp(ਲੱਭੋ_content((uint32_t)sfx.ਅਗੇਤਰ_2.untag().raw(), sfx.ਪੂਛ).ਟੈਗ(sfx.ਅਗੇਤਰ_2.is_ਜਨਤਕ()));
}

IdString TwinePool::ਜੋੜੋ(TwineSpec t) {
	ਜੇ (ਸਵੈਚਾਲਿਤ *ap = std::get_if<TwineSpec::AutoSuffix>(&t.ਅੰਕੜੇ))
		ਵਾਪਸ ਜੋੜੋ(auto_ਅਗੇਤਰ(ap->ਅਗੇਤਰ_2), ap->ਪੂਛ);
	ਜੇ (ਸਵੈਚਾਲਿਤ *ਪੱਤੀ_2 = std::get_if<TwineSpec::ਪੱਤੀ>(&t.ਅੰਕੜੇ))
		ਵਾਪਸ stamp(intern(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, ਪੱਤੀ_2->s));
	ਅਚਲ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ਅੰਕੜੇ);
	ਵਾਪਸ ਜੋੜੋ(sfx.ਅਗੇਤਰ_2, sfx.ਪੂਛ);
}

IdString TwinePool::auto_ਅਗੇਤਰ(ਅਚਲ std::string *ਅਗੇਤਰ_2) {
	ਸਵੈਚਾਲਿਤ it = auto_prefixes.ਲੱਭੋ(ਅਗੇਤਰ_2);
	ਜੇ (it != auto_prefixes.ਸਿਰਾ())
		ਵਾਪਸ it->ਦੂਜਾ;
	IdString ref = intern(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, *ਅਗੇਤਰ_2);
	auto_prefixes[ਅਗੇਤਰ_2] = ref;
	ਵਾਪਸ ref;
}

IdString TwinePool::ਜੋੜੋ(IdString ਅਗੇਤਰ_2, std::string_view ਪੂਛ) {
	ਵਾਪਸ stamp(intern((uint32_t)ਅਗੇਤਰ_2.untag().raw(), ਪੂਛ).ਟੈਗ(ਅਗੇਤਰ_2.is_ਜਨਤਕ()));
}

IdString TwinePool::ਜੋੜੋ(std::string_view s) {
	ਜੇ (s.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ IdString::Null;
	check_name(s);
	ਸਵੈਚਾਲਿਤ [content, is_ਜਨਤਕ_2] = unescape_name(s);
	ਵਾਪਸ stamp(intern(Twine_ਗੰਢ::NO_ਅਗੇਤਰ, content).ਟੈਗ(is_ਜਨਤਕ_2));
}

IdString TwinePool::copy_from(ਅਚਲ TwinePool& src, IdString ref) {
	IdString ਮਿਲਿਆ = ਲੱਭੋ_from(src, ref);
	ਜੇ (ਮਿਲਿਆ != IdString::Null || ref == IdString::Null)
		ਵਾਪਸ ਮਿਲਿਆ;
	ਅਚਲ Twine_ਗੰਢ& t = src[ref.untag()];
	ਜੇ (t.is_dead())
		ਵਾਪਸ IdString::Null;
	uint32_t ਅਗੇਤਰ_2 = Twine_ਗੰਢ::NO_ਅਗੇਤਰ;
	ਜੇ (t.is_suffix())
		ਅਗੇਤਰ_2 = (uint32_t)copy_from(src, t.ਅਗੇਤਰ_2()).untag().raw();
	ਵਾਪਸ stamp(intern(ਅਗੇਤਰ_2, t.ਪਾਠ()).ਟੈਗ(ref.is_ਜਨਤਕ()));
}

std::string TwinePool::ref_token(IdString ref) ਅਚਲ {
	ਵਾਪਸ "#" + std::ਮੰਜ਼ਿਲ_string((uint64_t)serial_) + ":"
		+ std::ਮੰਜ਼ਿਲ_string((uint64_t)stamp(ref).ਬਿੱਟ());
}

IdString TwinePool::ref_from_token(std::string_view token) ਅਚਲ {
	ਜੇ (token.ਮਾਪ() < 2 || token[0] != '#')
		ਵਾਪਸ IdString::Null;
	ਮਾਪ_t sep = token.ਲੱਭੋ(':');
	ਜੇ (sep == std::string_view::npos)
		ਵਾਪਸ IdString::Null;
	ਮਾਪ_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	ਲਈ (ਪੂਰਨ i = 0; i < 2; i++) {
		ਜੇ (parts[i].ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ IdString::Null;
		ਲਈ (ਅੱਖਰ c : parts[i]) {
			ਜੇ (c < '0' || c > '9')
				ਵਾਪਸ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ਜੇ (fields[0] != serial_)
		ਵਾਪਸ IdString::Null;
	IdString ref(fields[1]);
	ਜੇ (ref == IdString::Null)
		ਵਾਪਸ IdString::Null;
	ਵਾਪਸ is_ਜੀਵਤ(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ਬਣਤਰ TwineSegments {
	ਸਥਿਰ constexpr ਮਾਪ_t SEGMENT_INLINE_ਡੂੰਘਾਈ = 8;
	TwineSegments(ਅਚਲ TwinePool &pool, IdString ref);
	TwineSegments(ਅਚਲ TwinePool &pool, IdString ਸਿਰ, std::string_view ਪੂਛ);
	std::string_view peek();
	ਖਾਲੀ advance(ਮਾਪ_t n);
	ਮਾਪ_t ਕੁੱਲ_ਮਾਪ() ਅਚਲ;
ਨਿਜੀ:
	ਖਾਲੀ ਧੱਕੋ(std::string_view seg);
	std::string_view *segs();
	ਅਚਲ std::string_view *segs() ਅਚਲ;
	std::string_view inline_segs[SEGMENT_INLINE_ਡੂੰਘਾਈ];
	std::ਸਦਿਸ਼<std::string_view> spill;
	ਮਾਪ_t ਗਿਣਤੀ_2 = 0;
	ਮਾਪ_t pos = 0;
	ਮਾਪ_t off = 0;
};

TwineSegments::TwineSegments(ਅਚਲ TwinePool &pool, IdString ref)
{
	ਜੇ (ref == IdString::Null)
		ਵਾਪਸ;
	ਲਈ (IdString cur = ref.untag(); ;) {
		ਅਚਲ Twine_ਗੰਢ &t = pool[cur];
		ਚੋਣ (t.ਕਿਸਮ()) {
		ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Suffix:
			ਧੱਕੋ(t.ਪਾਠ());
			cur = t.ਅਗੇਤਰ_2();
			ਜਾਰੀ;
		ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::ਪੱਤੀ:
			ਧੱਕੋ(t.ਪਾਠ());
			ਰੋਕੋ;
		ਹਾਲਤ Twine_ਗੰਢ::ਕਿਸਮ_2::Dead:
			ਰੋਕੋ;
		}
		ਰੋਕੋ;
	}
	ਜੇ (ref.is_ਜਨਤਕ())
		ਧੱਕੋ("\\");
	std::ਉਲਟ(segs(), segs() + ਗਿਣਤੀ_2);
}

TwineSegments::TwineSegments(ਅਚਲ TwinePool &pool, IdString ਸਿਰ, std::string_view ਪੂਛ)
	: TwineSegments(pool, ਸਿਰ)
{
	ਧੱਕੋ(ਪੂਛ);
}

std::string_view TwineSegments::peek()
{
	ਜਦੋਂ (pos < ਗਿਣਤੀ_2) {
		std::string_view seg = segs()[pos];
		ਜੇ (off < seg.ਮਾਪ())
			ਵਾਪਸ seg.substr(off);
		pos++;
		off = 0;
	}
	ਵਾਪਸ {};
}

ਖਾਲੀ TwineSegments::advance(ਮਾਪ_t n) { off += n; }

ਮਾਪ_t TwineSegments::ਕੁੱਲ_ਮਾਪ() ਅਚਲ
{
	ਮਾਪ_t ਕੁੱਲ = 0;
	ਲਈ (ਮਾਪ_t i = 0; i < ਗਿਣਤੀ_2; i++)
		ਕੁੱਲ += segs()[i].ਮਾਪ();
	ਵਾਪਸ ਕੁੱਲ;
}

ਖਾਲੀ TwineSegments::ਧੱਕੋ(std::string_view seg)
{
	ਜੇ (spill.ਖਾਲੀ_ਥਾਵਾਂ() && ਗਿਣਤੀ_2 < SEGMENT_INLINE_ਡੂੰਘਾਈ) {
		inline_segs[ਗਿਣਤੀ_2++] = seg;
		ਵਾਪਸ;
	}
	ਜੇ (spill.ਖਾਲੀ_ਥਾਵਾਂ())
		spill.ਸੌਂਪੋ(inline_segs, inline_segs + ਗਿਣਤੀ_2);
	spill.ਧੱਕੋ_ਪਿੱਠ(seg);
	ਗਿਣਤੀ_2++;
}

std::string_view *TwineSegments::segs() { ਵਾਪਸ spill.ਖਾਲੀ_ਥਾਵਾਂ() ? inline_segs : spill.ਅੰਕੜੇ(); }
ਅਚਲ std::string_view *TwineSegments::segs() ਅਚਲ { ਵਾਪਸ spill.ਖਾਲੀ_ਥਾਵਾਂ() ? inline_segs : spill.ਅੰਕੜੇ(); }

ਮਾਪ_t TwinePool::str_ਮਾਪ(IdString ref) ਅਚਲ
{
	ਵਾਪਸ TwineSegments(*ਇਹ, ref).ਕੁੱਲ_ਮਾਪ();
}

ਤਾਰਕਿਕ TwinePool::begins_with(IdString ref, std::string_view ਅਗੇਤਰ_2) ਅਚਲ
{
	TwineSegments segs(*ਇਹ, ref);
	ਜਦੋਂ (!ਅਗੇਤਰ_2.ਖਾਲੀ_ਥਾਵਾਂ()) {
		std::string_view seg = segs.peek();
		ਜੇ (seg.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ ਝੂਠ;
		ਮਾਪ_t n = std::min(seg.ਮਾਪ(), ਅਗੇਤਰ_2.ਮਾਪ());
		ਜੇ (std::memcmp(seg.ਅੰਕੜੇ(), ਅਗੇਤਰ_2.ਅੰਕੜੇ(), n) != 0)
			ਵਾਪਸ ਝੂਠ;
		segs.advance(n);
		ਅਗੇਤਰ_2.remove_ਅਗੇਤਰ(n);
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ TwinePool::name_equal(IdString ref, std::string_view name) ਅਚਲ
{
	ਵਾਪਸ str_ਮਾਪ(ref) == name.ਮਾਪ() && begins_with(ref, name);
}

ਸਥਿਰ ਪੂਰਨ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ਜਦੋਂ (ਸੱਚ) {
		std::string_view x = sa.peek(), y = sb.peek();
		ਜੇ (x.ਖਾਲੀ_ਥਾਵਾਂ() || y.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ x.ਖਾਲੀ_ਥਾਵਾਂ() ? (y.ਖਾਲੀ_ਥਾਵਾਂ() ? 0 : -1) : 1;
		ਮਾਪ_t n = std::min(x.ਮਾਪ(), y.ਮਾਪ());
		ਜੇ (ਪੂਰਨ diff = std::memcmp(x.ਅੰਕੜੇ(), y.ਅੰਕੜੇ(), n); diff != 0)
			ਵਾਪਸ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::ਲੱਭੋ_content(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ, uint32_t hash) ਅਚਲ {
	ਵਾਪਸ ਲੱਭੋ_hashed(hash, [&](IdString ref) {
		ਅਚਲ Twine_ਗੰਢ &t = (*ਇਹ)[ref];
		ਜੇ (t == Twine_ਗੰਢ::ਕੁੰਜੀ_2{ਅਗੇਤਰ_2, ਪਾਠ})
			ਵਾਪਸ ਸੱਚ;
		IdString ਸਿਰ = ਅਗੇਤਰ_2 == Twine_ਗੰਢ::NO_ਅਗੇਤਰ ? IdString::Null : IdString(ਅਗੇਤਰ_2);
		TwineSegments sa(*ਇਹ, ref), sb(*ਇਹ, ਸਿਰ, ਪਾਠ);
		ਵਾਪਸ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::ਲੱਭੋ_from(ਅਚਲ TwinePool& src, IdString ref) ਅਚਲ {
	ਜੇ (ref == IdString::Null || ID::is_static(ref))
		ਵਾਪਸ ref;
	IdString idx = ref.untag();
	ਅਚਲ Twine_ਗੰਢ& t = src[idx];
	ਜੇ (t.is_dead())
		ਵਾਪਸ IdString::Null;
	IdString ਮਿਲਿਆ = ਲੱਭੋ_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ਇਹ, cand), sb(src, idx);
		ਵਾਪਸ compare_segments(sa, sb) == 0;
	});
	ਵਾਪਸ stamp(ਮਿਲਿਆ.ਟੈਗ(ref.is_ਜਨਤਕ()));
}

IdString TwinePool::ਅਗੇਤਰ_ਦਾ(IdString ref) ਅਚਲ {
	ਅਚਲ Twine_ਗੰਢ &t = (*ਇਹ)[ref];
	ਜੇ (!t.is_suffix())
		ਵਾਪਸ IdString::Null;
	ਵਾਪਸ stamp(t.ਅਗੇਤਰ_2().ਟੈਗ(ref.is_ਜਨਤਕ()));
}

ਪੂਰਨ TwinePool::compare_by_name(IdString a, IdString b) ਅਚਲ
{
	ਜੇ (a == b)
		ਵਾਪਸ 0;
	ਜੇ (a == IdString::Null)
		ਵਾਪਸ -1;
	ਜੇ (b == IdString::Null)
		ਵਾਪਸ 1;

	ਜੇ (a.is_ਜਨਤਕ() == b.is_ਜਨਤਕ()) {
		ਅਚਲ Twine_ਗੰਢ &ta = (*ਇਹ)[a.untag()];
		ਅਚਲ Twine_ਗੰਢ &tb = (*ਇਹ)[b.untag()];
		ਜੇ (ta.is_ਪੱਤੀ() && tb.is_ਪੱਤੀ())
			ਵਾਪਸ ta.ਪਾਠ().compare(tb.ਪਾਠ());
		ਜੇ (ta.is_suffix() && tb.is_suffix() && ta.ਅਗੇਤਰ_2() == tb.ਅਗੇਤਰ_2())
			ਵਾਪਸ ta.ਪਾਠ().compare(tb.ਪਾਠ());
	}

	TwineSegments sa(*ਇਹ, a), sb(*ਇਹ, b);
	ਵਾਪਸ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_ਸਿਰਾ
